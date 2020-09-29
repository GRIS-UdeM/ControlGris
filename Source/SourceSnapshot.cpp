#include "SourceSnapshot.h"

#include "Source.h"

SourceSnapshot::SourceSnapshot(Source const & source) noexcept : position(source.getPos()), z(source.getElevation())
{
}

SourceSnapshot const & SourcesSnapshots::operator[](SourceIndex const index) const
{
    jassert(index.toInt() >= 0 && index.toInt() < secondaries.size() + 1);
    if (index.toInt() == 0) {
        return primary;
    }
    return secondaries.getReference(index.toInt() - 1);
}

SourceSnapshot & SourcesSnapshots::operator[](SourceIndex const index)
{
    jassert(index.toInt() >= 0 && index.toInt() < secondaries.size() + 1);
    if (index.toInt() == 0) {
        return primary;
    }
    return secondaries.getReference(index.toInt() - 1);
}
