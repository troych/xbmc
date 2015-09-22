#pragma once
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

#include "DatabaseQuery.h"

class CDatabaseQueryRuleSQL : public CDatabaseQueryRule
{
public:
  virtual ~CDatabaseQueryRuleSQL(void) { }

  virtual std::string         GetWhereClause(const CDatabase &db, const std::string& strType) const override;

protected:
  std::string                 ValidateParameter(const std::string &parameter) const;
  virtual std::string         FormatParameter(const std::string &negate, const std::string &oper, const CDatabase &db, const std::string &type) const;
  virtual std::string         FormatWhereClause(const std::string &negate, const std::string &oper, const std::string &param,
                                                const CDatabase &db, const std::string &type) const;
  virtual std::string         GetOperatorString(SEARCH_OPERATOR op) const;
};

class CDatabaseQueryRuleCombinationSQL : public CDatabaseQueryRuleCombination
{
public:
  virtual ~CDatabaseQueryRuleCombinationSQL(void) { }

  virtual std::string GetWhereClause(const CDatabase &db, const std::string& strType) const override;
};
