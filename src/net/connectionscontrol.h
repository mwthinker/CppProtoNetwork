#ifndef NET_CONNECTIONSCONTROL_H
#define NET_CONNECTIONSCONTROL_H

#include "connection.h"

#include <memory>

namespace net {

    class ConnectionsControl {
    public:
		virtual ~ConnectionsControl() = default;

		// Thread safe.
		virtual std::shared_ptr<Connection> pollConnection() = 0;

        // Thread safe. Closes the the thread as fast as it can.
        virtual void close() = 0;

        // Thread safe.
		virtual void setAcceptConnections(bool accept) = 0;

        // Thread safe.
		virtual bool isAcceptingConnections() const = 0;
    };

} // Namespace net.

#endif // NET_CONNECTIONSCONTROL_H
