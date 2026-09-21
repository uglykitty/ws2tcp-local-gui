#ifndef CUSTOMRULES_H
#define CUSTOMRULES_H

#include <QString>
#include <QStringList>

// The custom routing rules: text with one domain per line, in the format the
// Rust core reads from its custom domain rules file. Blank lines and anything
// after a `#` are ignored, and a domain also covers its subdomains. The core
// silently skips a line that is not a domain, so these helpers follow its
// rules to let the GUI tell the user which lines will not take effect.
namespace CustomRules {

// The lowercase domain a line stands for, or an empty string when the core
// would ignore it: blank, comment only, or not a plain domain name (an IP
// address, a host with a port, wildcard, ...). `.example.com` and
// `example.com` are the same rule.
QString domainOf(const QString &line);

// True when the line is neither blank nor a comment but is not a domain either.
bool isIgnoredEntry(const QString &line);

// The number of distinct domains in the text.
int domainCount(const QString &text);

// The number of lines that are neither blank, a comment nor a domain.
int ignoredLineCount(const QString &text);

// Adds the domains of `incoming` that `existing` does not have to the end of
// `existing` and returns the result; the text already there is kept as it is.
// Comments and lines that are not domains in `incoming` are not carried over.
// `added` receives the number of domains appended.
QString merge(const QString &existing, const QString &incoming, int *added);

// The rules with line endings normalized to `\n`, a leading byte order mark
// removed and a single trailing newline, or an empty string for text that has
// nothing but blank lines.
QString normalized(const QString &text);

// A block of `#` comments that explains the format, with sample entries, to
// prefill an empty editor. It contains no active rule.
QString exampleText();

}  // namespace CustomRules

#endif
