/***************************************************************************
            ulxr_responseparse_wb.cpp  -  parse wbxml-rpc method response
                             -------------------
    begin                : Fri Jan 09 2004
    copyright            : (C) 2002-2007 by Ewald Arnold
    email                : ulxmlrpcpp@ewald-arnold.de

    $Id: ulxr_responseparse_wb.cpp 940 2006-12-30 18:22:05Z ewald-arnold $

 ***************************************************************************/

/**************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ***************************************************************************/

//#define ULXR_SHOW_TRACE
//#define ULXR_DEBUG_OUTPUT

#define ULXR_NEED_EXPORTS
#include <ulxmlrpcpp/ulxmlrpcpp.h>  // always first header

#include <ulxmlrpcpp/ulxr_responseparse_wb.h>
#include <ulxmlrpcpp/ulxr_except.h>
#include <ulxmlrpcpp/ulxr_htmlform_handler.h>


namespace ulxr {


ULXR_API_IMPL(void)
  MethodResponseParserWb::startElement(unsigned token, const Attributes &attr)
{
  if (!testStartElement(token, attr))
    ValueParserWb::testStartElement(token, attr);
}


ULXR_API_IMPL(bool)
  MethodResponseParserWb::testStartElement(unsigned token, const Attributes &)
{
  ULXR_TRACE(ULXR_PCHAR("MethodResponseParserWb::testStartElement()")
             << ULXR_PCHAR("\n  token: ")+HtmlFormHandler::makeHexNumber(token)
            );

  switch(states.top()->getParserState() )
  {
    case eNone:
      if(wbToken_MethodResponse == token)
      {
        setComplete (false);
        states.push(new ValueState(eMethodResponse));
      }
      else
        return false;
    break;

    case eMethodResponse:
      if(wbToken_Fault == token)
        states.push(new ValueState(eFault));

      else if(wbToken_Params == token)
        states.push(new ValueState(eParams));

      else
        return false;
    break;

    case eFault:
      if(wbToken_Value == token)
        states.push(new ValueState(eValue));
      else
        return false;
    break;

    case eParams:
      if(wbToken_Param == token)
        states.push(new ValueState(eParam));
      else
        return false;
    break;

    case eParam:
      if(wbToken_Value == token)
        states.push(new ValueState(eValue));
      else
        return false;
    break;

    default:
        return false;
  }

  return true;
}


ULXR_API_IMPL(void) MethodResponseParserWb::endElement()
{
  if (!testEndElement())
    ValueParserWb::testEndElement();
}


ULXR_API_IMPL(bool) MethodResponseParserWb::testEndElement()
{
  ULXR_TRACE(ULXR_PCHAR("MethodResponseParserWb::testEndElement()"));

  if (states.size() <= 1)
    throw RuntimeException(ApplicationError, ulxr_i18n(ULXR_PCHAR("abnormal program behaviour: MethodResponseParserWb::testEndElement() had no states left")));

  ValueState *curr = getTopValueState();
  states.pop();
  ValueState *on_top = getTopValueState();

  switch(curr->getParserState() )
  {
    case eMethodResponse:
      setComplete(true);
      on_top->takeValue (curr->getValue());
      if (on_top->getValue() != 0)
        method_value = *on_top->getValue();
    break;

    case eFault:
    case eParams:
    case eParam:
      on_top->takeValue (curr->getValue());
    break;

    default:
      states.push(curr);   // put back, someone else will process
      return false;
  }

  delete curr;
  return true;
}


}  // namespace ulxr
