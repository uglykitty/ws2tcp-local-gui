#ifndef UPSTREAMPROXY_H
#define UPSTREAMPROXY_H

#include <QNetworkProxy>
#include <QString>

// The upstream proxy setting: a http://, socks5h:// or socks5:// URL. The
// Rust core makes the tunnel and direct connections through it; these helpers
// let the GUI's own network requests follow the same proxy.
namespace UpstreamProxy {

// Blank is valid and means no proxy. Otherwise a URL with one of the schemes
// above, a host and no path, like ws2tcp-local-core accepts.
bool isValid(const QString &text);

// The proxy for a QNetworkAccessManager. Blank or invalid text gives
// QNetworkProxy::NoProxy. socks5:// resolves hostnames locally, socks5h:// and
// http:// leave that to the proxy.
QNetworkProxy toNetworkProxy(const QString &text);

}  // namespace UpstreamProxy

#endif
