#include "CustomRules.h"

#include <QCoreApplication>
#include <QHostAddress>
#include <QRegularExpression>
#include <QSet>

namespace CustomRules {

namespace {

QStringList splitLines(const QString &text) {
  return text.split(QRegularExpression(QStringLiteral("\r\n|\r|\n")));
}

bool isDomainChar(QChar c) {
  const char16_t u = c.unicode();
  return (u >= u'a' && u <= u'z') || (u >= u'A' && u <= u'Z') ||
         (u >= u'0' && u <= u'9') || u == u'-' || u == u'.';
}

QString withoutComment(const QString &line) {
  return line.section(QLatin1Char('#'), 0, 0).trimmed();
}

}  // namespace

QString domainOf(const QString &line) {
  QString domain = withoutComment(line);
  while (domain.startsWith(QLatin1Char('.'))) {
    domain.remove(0, 1);
  }
  while (domain.endsWith(QLatin1Char('.'))) {
    domain.chop(1);
  }
  if (domain.isEmpty()) {
    return {};
  }
  for (const QChar c : domain) {
    if (!isDomainChar(c)) {
      return {};
    }
  }
  QHostAddress address;
  if (address.setAddress(domain)) {
    return {};
  }
  return domain.toLower();
}

bool isIgnoredEntry(const QString &line) {
  return !withoutComment(line).isEmpty() && domainOf(line).isEmpty();
}

int domainCount(const QString &text) {
  QSet<QString> domains;
  for (const QString &line : splitLines(text)) {
    const QString domain = domainOf(line);
    if (!domain.isEmpty()) {
      domains.insert(domain);
    }
  }
  return domains.size();
}

int ignoredLineCount(const QString &text) {
  int count = 0;
  for (const QString &line : splitLines(text)) {
    if (isIgnoredEntry(line)) {
      ++count;
    }
  }
  return count;
}

QString merge(const QString &existing, const QString &incoming, int *added) {
  QSet<QString> known;
  for (const QString &line : splitLines(existing)) {
    const QString domain = domainOf(line);
    if (!domain.isEmpty()) {
      known.insert(domain);
    }
  }

  QStringList fresh;
  for (const QString &line : splitLines(incoming)) {
    const QString domain = domainOf(line);
    if (!domain.isEmpty() && !known.contains(domain)) {
      known.insert(domain);
      fresh.append(domain);
    }
  }
  if (added != nullptr) {
    *added = static_cast<int>(fresh.size());
  }
  if (fresh.isEmpty()) {
    return existing;
  }

  QString result = existing;
  if (!result.isEmpty() && !result.endsWith(QLatin1Char('\n'))) {
    result += QLatin1Char('\n');
  }
  result += fresh.join(QLatin1Char('\n'));
  result += QLatin1Char('\n');
  return result;
}

QString normalized(const QString &text) {
  QString result = text;
  if (result.startsWith(QChar(0xFEFF))) {
    result.remove(0, 1);
  }
  result.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
  result.replace(QLatin1Char('\r'), QLatin1Char('\n'));
  while (!result.isEmpty() && result.back().isSpace()) {
    result.chop(1);
  }
  if (result.isEmpty()) {
    return {};
  }
  return result + QLatin1Char('\n');
}

QString exampleText() {
  const auto t = [](const char *source) {
    return QCoreApplication::translate("CustomRules", source);
  };
  return QStringList{
      t("# Custom routing rules: these domains always go through the proxy."),
      t("# Write one domain per line. A domain also covers its subdomains, so"),
      t("# example.com matches www.example.com and api.example.com too."),
      t("# Blank lines, and anything after a # on a line, are ignored."),
      t("# IP addresses, ports and wildcards are not supported."),
      QStringLiteral("#"),
      t("# Examples (delete the leading # to turn one on):"),
      QStringLiteral("# example.com"),
      QStringLiteral("# .example.org"),
      QStringLiteral("# github.io  # text after # is a comment"),
      QString(),
  }.join(QLatin1Char('\n'));
}

}  // namespace CustomRules
