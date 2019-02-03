// This file is part of xsonrpc, an XML/JSON RPC library.
// Copyright (C) 2015 Erik Johansson <erik@ejohansson.se>
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "response.h"

#include "fault.h"
#include "writer.h"

namespace xsonrpc {

Response::Response(Value value, Value id)
  : myResult(std::move(value)),
    myIsFault(false),
    myFaultCode(0),
    myId(std::move(id))
{
}

Response::Response(int32_t faultCode, std::string faultString, Value id)
  : myIsFault(true),
    myFaultCode(faultCode),
    myFaultString(std::move(faultString)),
    myId(std::move(id))
{
}

void Response::Write(Writer& writer) const
{
  writer.StartDocument();
  if (myIsFault) {
    writer.StartFaultResponse(myId);
    writer.WriteFault(myFaultCode, myFaultString);
    writer.EndFaultResponse();
  }
  else {
    writer.StartResponse(myId);
    myResult.Write(writer);
    writer.EndResponse();
  }
  writer.EndDocument();
}

void Response::ThrowIfFault() const
{
  if (!IsFault()) {
    return;
  }

  switch (static_cast<Fault::ReservedCodes>(myFaultCode)) {
    case Fault::RESERVED_CODE_MIN:
    case Fault::RESERVED_CODE_MAX:
    case Fault::SERVER_ERROR_CODE_MIN:
      break;
    case Fault::PARSE_ERROR:
      throw ParseErrorFault(myFaultString);
    case Fault::INVALID_REQUEST:
      throw InvalidRequestFault(myFaultString);
    case Fault::METHOD_NOT_FOUND:
      throw MethodNotFoundFault(myFaultString);
    case Fault::INVALID_PARAMETERS:
      throw InvalidParametersFault(myFaultString);
    case Fault::INTERNAL_ERROR:
      throw InternalErrorFault(myFaultString);
  }

  if (myFaultCode >= Fault::SERVER_ERROR_CODE_MIN
      && myFaultCode <= Fault::SERVER_ERROR_CODE_MAX) {
    throw ServerErrorFault(myFaultCode, myFaultString);
  }

  if (myFaultCode >= Fault::RESERVED_CODE_MIN
      && myFaultCode <= Fault::RESERVED_CODE_MAX) {
    throw PreDefinedFault(myFaultCode, myFaultString);
  }

  throw Fault(myFaultString, myFaultCode);
}

} // namespace xsonrpc
