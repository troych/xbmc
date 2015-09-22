/*
 *      Copyright (C) 2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "DatabaseQuerySQL.h"
#include "Database.h"
#include "utils/StringUtils.h"
#include "XBDateTime.h"

using namespace std;

std::string CDatabaseQueryRuleSQL::ValidateParameter(const std::string &parameter) const
{
  if ((GetFieldType(m_field) == NUMERIC_FIELD ||
       GetFieldType(m_field) == SECONDS_FIELD) && parameter.empty())
    return "0"; // interpret empty fields as 0
  return parameter;
}

std::string CDatabaseQueryRuleSQL::FormatParameter(const std::string &operatorString, const std::string &param, const CDatabase &db, const std::string &strType) const
{
  std::string parameter;
  if (GetFieldType(m_field) == TEXTIN_FIELD)
  {
    vector<string> split = StringUtils::Split(param, ',');
    for (vector<string>::iterator itIn = split.begin(); itIn != split.end(); ++itIn)
    {
      if (!parameter.empty())
        parameter += ",";
      parameter += db.PrepareSQL("'%s'", StringUtils::Trim(*itIn).c_str());
    }
    parameter = " IN (" + parameter + ")";
  }
  else
    parameter = db.PrepareSQL(operatorString.c_str(), ValidateParameter(param).c_str());

  if (GetFieldType(m_field) == DATE_FIELD)
  {
    if (m_operator == OPERATOR_IN_THE_LAST || m_operator == OPERATOR_NOT_IN_THE_LAST)
    { // translate time period
      CDateTime date=CDateTime::GetCurrentDateTime();
      CDateTimeSpan span;
      span.SetFromPeriod(param);
      date-=span;
      parameter = db.PrepareSQL(operatorString.c_str(), date.GetAsDBDate().c_str());
    }
  }
  return parameter;
}

std::string CDatabaseQueryRuleSQL::GetOperatorString(SEARCH_OPERATOR op) const
{
  std::string operatorString;
  if (GetFieldType(m_field) != TEXTIN_FIELD)
  {
    // the comparison piece
    switch (op)
    {
    case OPERATOR_CONTAINS:
      operatorString = " LIKE '%%%s%%'"; break;
    case OPERATOR_DOES_NOT_CONTAIN:
      operatorString = " LIKE '%%%s%%'"; break;
    case OPERATOR_EQUALS:
      if (GetFieldType(m_field) == NUMERIC_FIELD || GetFieldType(m_field) == SECONDS_FIELD)
        operatorString = " = %s";
      else
        operatorString = " LIKE '%s'";
      break;
    case OPERATOR_DOES_NOT_EQUAL:
      if (GetFieldType(m_field) == NUMERIC_FIELD || GetFieldType(m_field) == SECONDS_FIELD)
        operatorString = " != %s";
      else
        operatorString = " LIKE '%s'";
      break;
    case OPERATOR_STARTS_WITH:
      operatorString = " LIKE '%s%%'"; break;
    case OPERATOR_ENDS_WITH:
      operatorString = " LIKE '%%%s'"; break;
    case OPERATOR_AFTER:
    case OPERATOR_GREATER_THAN:
    case OPERATOR_IN_THE_LAST:
      operatorString = " > ";
      if (GetFieldType(m_field) == NUMERIC_FIELD || GetFieldType(m_field) == SECONDS_FIELD)
        operatorString += "%s";
      else
        operatorString += "'%s'";
      break;
    case OPERATOR_BEFORE:
    case OPERATOR_LESS_THAN:
    case OPERATOR_NOT_IN_THE_LAST:
      operatorString = " < ";
      if (GetFieldType(m_field) == NUMERIC_FIELD || GetFieldType(m_field) == SECONDS_FIELD)
        operatorString += "%s";
      else
        operatorString += "'%s'";
      break;
    case OPERATOR_TRUE:
      operatorString = " = 1"; break;
    case OPERATOR_FALSE:
      operatorString = " = 0"; break;
    default:
      break;
    }
  }
  return operatorString;
}

std::string CDatabaseQueryRuleSQL::GetWhereClause(const CDatabase &db, const std::string& strType) const
{
  SEARCH_OPERATOR op = GetOperator(strType);

  std::string operatorString = GetOperatorString(op);
  std::string negate;
  if (op == OPERATOR_DOES_NOT_CONTAIN || op == OPERATOR_FALSE ||
     (op == OPERATOR_DOES_NOT_EQUAL && GetFieldType(m_field) != NUMERIC_FIELD && GetFieldType(m_field) != SECONDS_FIELD))
    negate = " NOT";

  // boolean operators don't have any values in m_parameter, they work on the operator
  if (m_operator == OPERATOR_FALSE || m_operator == OPERATOR_TRUE)
    return GetBooleanQuery(negate, strType);

  // The BETWEEN operator is handled special
  if (op == OPERATOR_BETWEEN)
  {
    if (m_parameter.size() != 2)
      return "";

    FIELD_TYPE fieldType = GetFieldType(m_field);
    if (fieldType == NUMERIC_FIELD)
      return db.PrepareSQL("CAST(%s as DECIMAL(5,1)) BETWEEN %s AND %s", GetField(m_field, strType).c_str(), m_parameter[0].c_str(), m_parameter[1].c_str());
    else if (fieldType == SECONDS_FIELD)
      return db.PrepareSQL("CAST(%s as INTEGER) BETWEEN %s AND %s", GetField(m_field, strType).c_str(), m_parameter[0].c_str(), m_parameter[1].c_str());
    else
      return db.PrepareSQL("%s BETWEEN '%s' AND '%s'", GetField(m_field, strType).c_str(), m_parameter[0].c_str(), m_parameter[1].c_str());
  }

  // now the query parameter
  std::string wholeQuery;
  for (vector<string>::const_iterator it = m_parameter.begin(); it != m_parameter.end(); ++it)
  {
    std::string query = '(' + FormatWhereClause(negate, operatorString, *it, db, strType) + ')';

    if (it + 1 != m_parameter.end())
    {
      if (negate.empty())
        query += " OR ";
      else
        query += " AND ";
    }

    wholeQuery += query;
  }

  return wholeQuery;
}

std::string CDatabaseQueryRuleSQL::FormatWhereClause(const std::string &negate, const std::string &oper, const std::string &param,
                                                 const CDatabase &db, const std::string &strType) const
{
  std::string parameter = FormatParameter(oper, param, db, strType);

  std::string query;
  if (m_field != 0)
  {
    string fmt = "%s";
    if (GetFieldType(m_field) == NUMERIC_FIELD)
      fmt = "CAST(%s as DECIMAL(5,1))";
    else if (GetFieldType(m_field) == SECONDS_FIELD)
      fmt = "CAST(%s as INTEGER)";

    query = StringUtils::Format(fmt.c_str(), GetField(m_field,strType).c_str());
    query += negate + parameter;

    // special case for matching parameters in fields that might be either empty or NULL.
    if ((  param.empty() &&  negate.empty() ) ||
        ( !param.empty() && !negate.empty() ))
      query += " OR " + GetField(m_field,strType) + " IS NULL";
  }

  if (query == negate + parameter)
    query = "1";
  return query;
}

std::string CDatabaseQueryRuleCombinationSQL::GetWhereClause(const CDatabase &db, const std::string& strType) const
{
  std::string rule;

  // translate the combinations into SQL
  for (CDatabaseQueryRuleCombinations::const_iterator it = m_combinations.begin(); it != m_combinations.end(); ++it)
  {
    if (it != m_combinations.begin())
      rule += m_type == CombinationAnd ? " AND " : " OR ";
    rule += "(" + (*it)->GetWhereClause(db, strType) + ")";
  }

  // translate the rules into SQL
  for (CDatabaseQueryRules::const_iterator it = m_rules.begin(); it != m_rules.end(); ++it)
  {
    if (!rule.empty())
      rule += m_type == CombinationAnd ? " AND " : " OR ";
    rule += "(";
    std::string currentRule = (*it)->GetWhereClause(db, strType);
    // if we don't get a rule, we add '1' or '0' so the query is still valid and doesn't fail
    if (currentRule.empty())
      currentRule = m_type == CombinationAnd ? "'1'" : "'0'";
    rule += currentRule;
    rule += ")";
  }

  return rule;
}
