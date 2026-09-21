#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QPlainTextEdit>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QToolButton>

#include <functional>

#include "ws2tcp_local_ffi.h"

#ifdef Q_OS_WIN
class QProcess;
#endif

class MainWindow final : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;
  bool clearUserSettingsAndQuit();
  void showAndActivate();
  void quitGracefully(bool confirmIfWslBusy = true);

 private slots:
  void startProxy();
  void stopProxy();
  void updateProxyMode(const QString &mode);
  void refreshStatus();
  void appendLog(QString message);
  void showSettingsDialog();
  void showAboutDialog();
  void showCustomRulesDialog();
  void checkForUpdates();
  void toggleWindowVisibility();
  void quitFromTray();
  void handleTrayActivation(QSystemTrayIcon::ActivationReason reason);
#ifdef WS2TCP_SYSTEM_PROXY_AVAILABLE
  void setSystemProxyEnabled(bool enabled);
#endif
#ifdef Q_OS_WIN
  void enableWslMirroredNetworking();
  void installWsl();
  void installNodeViaNvm();
  void installOpenCodeCli();
  void installCodexCli();
  void installClaudeCodeCli();
#endif

 protected:
  void closeEvent(QCloseEvent *event) override;

 private:
  QByteArray buildConfigJson(const QString &customRulesPath) const;
  void setupTrayIcon();
  void updateTrayActions();
  // Shows whether the proxy is running on the tray icon, its tooltip and the
  // first line of the tray menu.
  void updateTrayStatus();
  void updateConfigurationInputs(bool running);
  void updateCustomRulesSummary();
  // The file the custom rules are written to for the Rust core to read.
  static QString customRulesFilePath();
  // Writes the custom rules to customRulesFilePath() and returns its path in
  // `path`, which stays empty when there are no rules to apply.
  bool writeCustomRulesFile(QString *path, QString *error) const;
  void loadUserSettings();
  void saveUserSettings() const;
  // `startup` is true for the automatic check after launch: it stays quiet
  // unless a newer version with an installer for this platform exists.
  void runUpdateCheck(bool startup);
  void appendError(const QString &prefix);
  void showError(const QString &message);
  void showGatewayCheckFailure(const QString &message);
  void showInfo(const QString &message);
  void logMessage(const QString &message);
  void updateRuntimeStatus(const QString &message);
  void updateRuntimeStatusFromLog(const QString &message);
  static void handleRustLog(const char *message, void *userData);
#ifdef Q_OS_WIN
  void showRestartNotice(const QString &message, const QString &settingsKey,
                         bool *suppressed);
  void showEnvProxyRestartNotice();
  void maybePromptWslMirroredNetworking();
  void applyMirroredNetworking();
  void promptRestartAfterWslInstall();
  bool isWslUsable();
  void showWslNotReadyMessage();
  bool confirmQuitDuringWslOperation();
  void runWslCommand(const QString &label, const QStringList &arguments,
                     const QByteArray &stdinData = {},
                     std::function<void(bool)> onFinished = {});
  void runWslScript(const QString &label, const QString &resourcePath,
                    const QStringList &scriptArgs,
                    std::function<void(bool)> onFinished = {});
#endif

  Ws2TcpHandle *handle_ = nullptr;
  QLineEdit *listenEdit_ = nullptr;
  QLineEdit *socksListenEdit_ = nullptr;
  QLineEdit *gatewayEdit_ = nullptr;
  QLineEdit *usernameEdit_ = nullptr;
  QLineEdit *passwordEdit_ = nullptr;
  QToolButton *passwordVisibilityButton_ = nullptr;
  QLabel *customRulesSummary_ = nullptr;
  QToolButton *customRulesEditButton_ = nullptr;
  // One domain per line, edited in showCustomRulesDialog() and kept in the
  // settings. Empty until the user has saved rules.
  QString customRules_;
  QComboBox *proxyModeCombo_ = nullptr;
  QAction *startAction_ = nullptr;
  QAction *stopAction_ = nullptr;
  QAction *settingsAction_ = nullptr;
  QPlainTextEdit *logView_ = nullptr;
  QTimer *statusTimer_ = nullptr;
  QSystemTrayIcon *trayIcon_ = nullptr;
  QMenu *trayMenu_ = nullptr;
  QAction *showHideAction_ = nullptr;
  QAction *trayStatusAction_ = nullptr;
#ifdef WS2TCP_SYSTEM_PROXY_AVAILABLE
  QCheckBox *systemProxyCheck_ = nullptr;
  QAction *traySystemProxyAction_ = nullptr;
  bool systemProxyActive_ = false;
#endif
#ifdef Q_OS_WIN
  bool suppressEnvProxyNotice_ = false;
  bool suppressWslRestartNotice_ = false;
  bool suppressWslMirroredPrompt_ = false;
  QMenu *wslMenu_ = nullptr;
  QProcess *wslProcess_ = nullptr;
#endif
  QAction *quitAction_ = nullptr;
  bool wasRunning_ = false;
  // The proxy stopped by itself with an error, and has not been started or
  // stopped by the user since.
  bool lastStopFailed_ = false;
  // The state the tray icon currently shows, or -1 before it is first set.
  int trayState_ = -1;
  // The address the running proxy reported listening on, for the tray.
  QString activeListen_;
  bool allowClose_ = false;
  bool userSettingsCleared_ = false;
  int bufferSize_ = 16 * 1024;
  int refreshIntervalSeconds_ = 60;
  bool insecure_ = false;
  bool checkUpdatesOnStartup_ = true;
  bool updateCheckInProgress_ = false;
  // How to authenticate to the gateway: "token" (default) or "basic", the
  // compatibility method that is being phased out.
  QString authMode_ = QStringLiteral("token");
  // http://, socks5h:// or socks5:// URL of a proxy server that connections to
  // the gateway go through when upstreamProxyEnabled_ is set. The URL is kept
  // while the switch is off, so it can be turned back on without retyping.
  QString upstreamProxy_;
  bool upstreamProxyEnabled_ = false;
  QString closeBehavior_ = "ask";
  QString sessionCloseBehavior_;
  QString language_ = "en_US";
  QString runtimeStatus_;
};

#endif
