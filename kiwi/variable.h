/*-----------------------------------------------------------------------------
| Copyright (c) 2013-2026, Nucleic Development Team.
|
| Distributed under the terms of the Modified BSD License.
|
| The full license is in the file LICENSE, distributed with this software.
|----------------------------------------------------------------------------*/
#pragma once
#include <memory>
#include <string>
#include "shareddata.h"

namespace kiwi
{

class Variable
{
friend struct std::hash<kiwi::Variable>;
public:
    Variable(const char* debug_name = nullptr, uint64_t cookie = 0) : m_data(new VariableData(debug_name, cookie)) {}

    Variable(const Variable&) = default;

    Variable(Variable&&) noexcept = default;

    ~Variable() = default;

    const char* debugName() const
    {
        return m_data->m_debugName;
    }

    void setDebugName(const char *debug_name)
    {
        m_data->m_debugName = debug_name;
    }

    uint64_t cookie() const
    {
        return m_data->m_cookie;
    }

    void setContext(uint64_t cookie)
    {
        m_data->m_cookie = cookie;
    }

    double value() const
    {
        return m_data->m_value;
    }

    void setValue(double value)
    {
        m_data->m_value = value;
    }

    // operator== is used for symbolics
    bool equals(const Variable &other) const
    {
        return m_data == other.m_data;
    }

    Variable& operator=(const Variable&) = default;

    Variable& operator=(Variable&&) noexcept = default;

private:
    class VariableData : public SharedData
    {

    public:
        VariableData(const char* debug_name, uint64_t cookie) : SharedData(),
                                                           m_debugName( debug_name),
                                                           m_cookie(cookie),
                                                           m_value(0.0) {}

        ~VariableData() = default;

        const char* m_debugName;
        uint64_t    m_cookie;
        double      m_value;

    private:
        VariableData(const VariableData &other);

        VariableData &operator=(const VariableData &other);
    };

    SharedDataPtr<VariableData> m_data;

    friend bool operator<(const Variable &lhs, const Variable &rhs)
    {
        return lhs.m_data < rhs.m_data;
    }
};

} // namespace kiwi

namespace std
{
	template <>
	struct hash<kiwi::Variable>
	{
		std::size_t operator()( const kiwi::Variable& v ) const
		{
			return (std::size_t)v.m_data.data();
		}
	};
}