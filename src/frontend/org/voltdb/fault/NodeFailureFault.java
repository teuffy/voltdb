/* This file is part of VoltDB.
 * Copyright (C) 2008-2010 VoltDB L.L.C.
 *
 * VoltDB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VoltDB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VoltDB.  If not, see <http://www.gnu.org/licenses/>.
 */
package org.voltdb.fault;

public class NodeFailureFault extends VoltFault
{
    public static int NODE_FAILURE_FOREIGN_HOST = 0;
    public static int NODE_FAILURE_CATALOG = 1;
    public static int NODE_FAILURE_INITIATOR = 2;
    public static int NODE_FAILURE_EXECUTION_SITE = 3;

    public NodeFailureFault(int hostId, String hostname)
    {
        super(FaultType.NODE_FAILURE);
        m_hostId = hostId;
        m_hostname = hostname;
    }

    public int getHostId()
    {
        return m_hostId;
    }

    public String getHostname()
    {
        return m_hostname;
    }

    @Override
    public String toString()
    {
        StringBuilder sb = new StringBuilder();

        sb.append("NodeFailureFault:\n");
        sb.append("  Host: " + m_hostname + "\n");
        sb.append("  Host Id: " + m_hostId + "\n");
        sb.append(super.toString());

        return sb.toString();
    }

    @Override
    public boolean equals(Object other) {
        if (other instanceof NodeFailureFault) {
            NodeFailureFault ofault = (NodeFailureFault)other;
            if (ofault.getHostId() == m_hostId && ofault.getHostname().equals(m_hostname)) {
                return true;
            }
        }
        return false;
    }

    @Override
    public int hashCode() {
        return m_hostId + m_hostname.hashCode();
    }

    private int m_hostId;
    private String m_hostname;
}
