/* This file is part of VoltDB.
 * Copyright (C) 2008-2017 VoltDB Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with VoltDB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EXPORTTUPLESTREAM_H_
#define EXPORTTUPLESTREAM_H_
#include "common/ids.h"
#include "common/tabletuple.h"
#include "common/FatalException.hpp"
#include "storage/TupleStreamBase.h"
#include <deque>
#include <cassert>
namespace voltdb {

class StreamBlock;

class ExportTupleStream : public voltdb::TupleStreamBase {
public:
    enum Type { INSERT, DELETE };

    ExportTupleStream(CatalogId partitionId, int64_t siteId);

    virtual ~ExportTupleStream() {
    }

    void setSignatureAndGeneration(std::string signature, int64_t generation);

    /** Read the total bytes used over the life of the stream */
    size_t bytesUsed() {
        return m_uso;
    }

    /** Set the total number of bytes used (for rejoin/recover) */
    void setBytesUsed(size_t count) {
        assert(m_uso == 0);
        StreamBlock *sb = new StreamBlock(new char[1], 0, 0, count);
        ExecutorContext::getExecutorContext()->getTopend()->pushExportBuffer(
                                m_generation, m_partitionId, m_signature, sb, false, false);
        delete sb;
        m_uso = count;
        //Extend the buffer chain to replace any existing stream blocks with a new one
        //with the correct USO
        extendBufferChain(0);
    }

    inline size_t getTextStringSerializedSize(const std::string &value) const {
        return value.length() + sizeof(int32_t);
    }

    // compute # of bytes needed to serialize the meta data column names
    inline size_t getMDColumnNamesSerializedSize() const { return m_mdColumnNamesSerializedSize; }

    int64_t allocatedByteCount(bool debug = false) const {
        if (debug) {
            std::cout <<  "(m_pendingBlocks.size() " << m_pendingBlocks.size() << " * "
                    << "(m_defaultCapacity - m_headerSpace): " << (m_defaultCapacity - m_headerSpace)
                    << " ) + queued export bytes:"
                    << ExecutorContext::getExecutorContext()->getTopend()->getQueuedExportBytes(m_partitionId, m_signature) << std::endl;
        }
        return (m_pendingBlocks.size() * (m_defaultCapacity - m_headerSpace)) +
                ExecutorContext::getExecutorContext()->getTopend()->getQueuedExportBytes(m_partitionId, m_signature);
    }

    void pushExportBuffer(StreamBlock *block, bool sync, bool endOfStream);

    /** write a tuple to the stream */
    virtual size_t appendTuple(int64_t lastCommittedSpHandle,
                       int64_t spHandle,
                       int64_t seqNo,
                       int64_t uniqueId,
                       int64_t timestamp,
                       TableTuple &tuple,
                       std::vector<std::string> const& columnNames,
                       int partitionColumn,
                       ExportTupleStream::Type type);

    size_t computeOffsets(TableTuple &tuple,size_t *rowHeaderSz);

    virtual int partitionId() { return m_partitionId; }

private:
    // cached catalog values
    const CatalogId m_partitionId;
    const int64_t m_siteId;

    std::string m_signature;
    int64_t m_generation;
    const size_t m_mdColumnNamesSerializedSize;

    // meta-data column count
    static const int METADATA_COL_CNT = 6;

    // column names of meta data columns
    static const std::string VOLT_TRANSACTION_ID;
    static const std::string VOLT_EXPORT_TIMESTAMP;
    static const std::string VOLT_EXPORT_SEQUENCE_NUMBER;
    static const std::string VOLT_PARTITION_ID;
    static const std::string VOLT_SITE_ID;
    static const std::string VOLT_EXPORT_OPERATION;
};



}

#endif
