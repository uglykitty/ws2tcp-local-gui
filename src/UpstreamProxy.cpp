#include "UpstreamProxy.h"

#include <QUrl>

namespace UpstreamProxy {

bool isValid(const QString &text) {
  const QString trimmed = text.trimmed();
  if (trimmed.isEmpty()) {
    return true;
  }
  const QUrl url(trimmed, QUrl::StrictMode);
  const QString scheme = url.scheme();
  return url.isValid() &&
         (scheme == QLatin1String("http") ||
          scheme == QLatin1String("socks5") ||
          scheme == QLatin1String("socks5h")) &&
         !url.host().isEmpty() &&
         (url.path().isEmpty() || url.path() == QLatin1String("/")) &&
         !url.hasQuery() && !url.hasFragment();
}

QNetworkProxy toNetworkProxy(const QString &text) {
  const QString trimmed = text.trimmed();
  if (trimmed.isEmpty() || !isValid(trimmed)) {
    return QNetworkProxy(QNetworkProxy::NoProxy);
  }
  const QUrl url(trimmed, QUrl::StrictMode);
  const bool http = url.scheme() == QLatin1String("http");

  QNetworkProxy proxy(
      http ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy,
      url.host(), static_cast<quint16>(url.port(http ? 80 : 1080)),
      url.userName(), url.password());
  if (url.scheme() == QLatin1String("socks5")) {
    proxy.setCapabilities(proxy.capabilities() &
                          ~QNetworkProxy::HostNameLookupCapability);
  }
  return proxy;
}

}  // namespace UpstreamProxy
