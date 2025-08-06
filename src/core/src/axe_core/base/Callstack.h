#pragma once
// Ref: https://github.com/simco50/D3D12_Research/blob/a7e5b93fdad88e335c88b80bba96df9c8ddfd80e/Source/Core/Callstack.h

namespace axe {

struct Symbol
{
	u64  Address;
	char Name[255];
	char FilePath[MAX_PATH];
	u32  LineNumber;
};

namespace StackTrace
{
	u32 Trace(void** pStackData, u32 stackSize, u32 skipDepth);
	void Resolve(Span<u64> stackFrame, u32 numFrames, Symbol* outSymbols);
}

template<u32 Size>
class Callstack : public NonCopyable {
public:
	void Trace(u32 skipDepth = 0)
	{
		m_Resolved = false;
		m_NumFrames = StackTrace::Trace((void**)&m_Stack, Size, skipDepth + 1);
	}

	Span<const Symbol> Resolve()
	{
		if (!m_Resolved)
		{
			auto stackSpan = Span<u64>(m_Stack);
			StackTrace::Resolve(stackSpan, m_NumFrames, m_Symbols);
			m_Resolved = true;
		}
		return m_Symbols;
	}

	String ToString() {
		Resolve();
		String output;
		for (u32 i = 0; i < m_NumFrames; i++)
		{
			const Symbol& s = m_Symbols[i];
			output += TempString::s_format("0x{:x} - {}() - Line {}\n", s.Address, s.Name, s.LineNumber);
		}
		return output;
	}

private:
	bool	m_Resolved = false;
	u32		m_NumFrames = 0;
	Symbol	m_Symbols[Size];
	u64		m_Stack[Size];
};

} // namespace axe
