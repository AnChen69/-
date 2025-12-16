#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QShortcut>
#include <memory>
#include "../core/crypto/MessageEncryption.h"

using namespace std;

class ChatWindow : public QMainWindow {
    Q_OBJECT

public:
    ChatWindow(QWidget* parent = nullptr);

private slots:
    void onSendMessage();
    void onSendFile();
    void onShowKeyManager();
    void onEncryptionToggled(bool checked);
    void onUserSelected(QListWidgetItem* item);
    void testEncryption();

private:
    void setupUI();
    void setupConnections();
    void initializeEncryption();
    QString formatFileSize(qint64 bytes);
    void updateStatus(const QString& message);
    void addMessageToHistory(const QString& sender, const QString& message, bool isMe = true);

    // UI组件
    QTextEdit* messageInput;
    QTextEdit* chatHistory;
    QListWidget* userList;
    QPushButton* sendButton;
    QPushButton* fileButton;
    QPushButton* keyManagerButton;
    QPushButton* encryptToggleButton;
    QPushButton* testButton;
    QLabel* statusLabel;

    // 核心模块
    unique_ptr<MessageEncryption> encryptor;
    QString currentUser;
};