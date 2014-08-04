#pragma once
#include <iostream>
#include <string>


namespace Derp
{
    struct ErrorReporting
    {
        typedef std::wstring string_type;
        typedef std::wostringstream stream_type;

        virtual ~ErrorReporting(){}

        template<class MessageType, class... Args>
        void ReportError(size_t lineNumber, const MessageType& msg, const Args&... args)
        {
            stream_type out;
            internal_ReportError(Concat(out, "Error(", lineNumber, "): ", msg, args...).str());
        }

        template<class MessageType, class... Args>
        void ReportWarning(const MessageType& msg, const Args&... args)
        {
            stream_type out;
            internal_ReportError(Concat(out, "Warning: ", msg, args...).str());
        }

        template<class MessageType, class... Args>
        void ReportFatal(const MessageType& msg, const Args&... args)
        {
            stream_type out;
            internal_ReportError(Concat(out, "Internal Error: ", msg, args...).str());
        }

        template<typename Stream, typename ...Args>
        Stream& Concat(Stream& o, const Args&... args)
        {
            auto x = std::initializer_list<int>{ ((o << args), 0)... };
            return o;
        }

        virtual void internal_ReportError(const string_type& msg)
        {
            std::wcout << msg << std::endl;
        }

    };
}

