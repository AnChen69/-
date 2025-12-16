#include "ChatWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QShortcut>
#include <iostream>

using namespace std;

ChatWindow::ChatWindow(QWidget* parent) : QMainWindow(parent), currentUser("我") {
    setWindowTitle("安全通信工具 v1.0");
    resize(1000, 700);

    setupUI();
    setupConnections();
    initializeEncryption();
}

void ChatWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // 左侧：用户列表区域
    QWidget* leftPanel = new QWidget();
    leftPanel->setMaximumWidth(220);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    QLabel* userLabel = new QLabel("在线用户");
    userLabel->setStyleSheet("font-weight: bold; font-size: 16px; margin: 10px; color: #333;");
    leftLayout->addWidget(userLabel);

    userList = new QListWidget();
    userList->setStyleSheet(R"(
        QListWidget {
            border: 1px solid #ddd;
            border-radius: 5px;
            background-color: white;
            font-size: 13px;
        }
        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #eee;
        }
        QListWidget::item:hover {
            background-color: #f0f0f0;
        }
        QListWidget::item:selected {
            background-color: #4CAF50;
            color: white;
        }
    )");

    QStringList users = { "张三 (在线)", "李四 (离线)", "王五 (在线)", "赵六 (忙碌)" };
    userList->addItems(users);
    leftLayout->addWidget(userList);

    keyManagerButton = new QPushButton("密钥管理");
    keyManagerButton->setStyleSheet(R"(
        QPushButton {
            background-color: #FF9800;
            color: white;
            border-radius: 4px;
            padding: 10px;
            font-weight: bold;
            margin-top: 10px;
        }
        QPushButton:hover {
            background-color: #F57C00;
        }
    )");
    leftLayout->addWidget(keyManagerButton);

    testButton = new QPushButton("测试加密");
    testButton->setStyleSheet(R"(
        QPushButton {
            background-color: #9C27B0;
            color: white;
            border-radius: 4px;
            padding: 10px;
            font-weight: bold;
            margin-top: 5px;
        }
        QPushButton:hover {
            background-color: #7B1FA2;
        }
    )");
    leftLayout->addWidget(testButton);

    leftLayout->addStretch();

    // 右侧：聊天区域
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    QLabel* chatLabel = new QLabel("聊天记录");
    chatLabel->setStyleSheet("font-weight: bold; font-size: 16px; margin: 5px; color: #333;");
    rightLayout->addWidget(chatLabel);

    chatHistory = new QTextEdit();
    chatHistory->setReadOnly(true);
    chatHistory->setStyleSheet(R"(
        QTextEdit {
            border: 1px solid #ddd;
            border-radius: 8px;
            padding: 15px;
            background-color: #f9f9f9;
            font-family: "Microsoft YaHei", sans-serif;
            font-size: 14px;
            line-height: 1.5;
        }
    )");

    chatHistory->append("欢迎使用安全通信工具！");
    chatHistory->append("加密模块已就绪");
    chatHistory->append("请在左侧选择聊天对象");
    chatHistory->append("");

    rightLayout->addWidget(chatHistory, 1);

    // 输入区域
    QWidget* inputWidget = new QWidget();
    QVBoxLayout* inputLayout = new QVBoxLayout(inputWidget);

    QLabel* inputLabel = new QLabel("输入消息");
    inputLabel->setStyleSheet("font-weight: bold; margin: 5px; color: #333;");
    inputLayout->addWidget(inputLabel);

    messageInput = new QTextEdit();
    messageInput->setMaximumHeight(100);
    messageInput->setPlaceholderText("输入要发送的消息...（Ctrl+Enter发送）");
    messageInput->setStyleSheet(R"(
        QTextEdit {
            border: 2px solid #4CAF50;
            border-radius: 6px;
            padding: 10px;
            font-size: 14px;
            font-family: "Microsoft YaHei", sans-serif;
        }
        QTextEdit:focus {
            border: 2px solid #2196F3;
        }
    )");
    inputLayout->addWidget(messageInput);

    // 按钮区域
    QWidget* buttonWidget = new QWidget();
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);

    fileButton = new QPushButton("发送文件");
    fileButton->setStyleSheet(R"(
        QPushButton {
            background-color: #2196F3;
            color: white;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #1976D2;
        }
    )");

    encryptToggleButton = new QPushButton("加密模式");
    encryptToggleButton->setCheckable(true);
    encryptToggleButton->setChecked(true);
    encryptToggleButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:checked {
            background-color: #45a049;
        }
        QPushButton:!checked {
            background-color: #FF9800;
        }
    )");

    sendButton = new QPushButton("发送");
    sendButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border-radius: 6px;
            padding: 10px 30px;
            font-size: 16px;
            font-weight: bold;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
    )");

    buttonLayout->addWidget(fileButton);
    buttonLayout->addWidget(encryptToggleButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(sendButton);

    inputLayout->addWidget(buttonWidget);
    rightLayout->addWidget(inputWidget);

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightPanel, 1);

    statusBar()->setStyleSheet("background-color: #e8e8e8; color: #333; font-size: 12px;");
    statusLabel = new QLabel("就绪 | 加密模式: 开启 | 选择聊天对象开始对话");
    statusBar()->addWidget(statusLabel);
}

void ChatWindow::setupConnections() {
    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::onSendMessage);
    connect(fileButton, &QPushButton::clicked, this, &ChatWindow::onSendFile);
    connect(keyManagerButton, &QPushButton::clicked, this, &ChatWindow::onShowKeyManager);
    connect(encryptToggleButton, &QPushButton::toggled, this, &ChatWindow::onEncryptionToggled);
    connect(testButton, &QPushButton::clicked, this, &ChatWindow::testEncryption);
    connect(userList, &QListWidget::itemClicked, this, &ChatWindow::onUserSelected);

    QShortcut* sendShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
    connect(sendShortcut, &QShortcut::activated, this, &ChatWindow::onSendMessage);
}

void ChatWindow::initializeEncryption() {
    try {
        encryptor = make_unique<MessageEncryption>();
        if (encryptor->isInitialized()) {
            updateStatus("加密模块初始化成功 | AES-256密钥已生成");
            chatHistory->append("加密模块初始化成功");
        }
        else {
            updateStatus("警告：加密模块初始化失败");
            chatHistory->append("加密模块初始化失败，将使用明文模式");
        }
    }
    catch (const exception& e) {
        QMessageBox::warning(this, "加密模块错误",
            QString("加密模块初始化失败: %1").arg(e.what()));
        updateStatus("错误：加密模块初始化失败 - " + QString(e.what()));
    }
}

void ChatWindow::onSendMessage() {
    QString text = messageInput->toPlainText().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "消息内容不能为空！");
        return;
    }

    QString selectedUser = "未知用户";
    QList<QListWidgetItem*> selectedItems = userList->selectedItems();
    if (!selectedItems.isEmpty()) {
        selectedUser = selectedItems.first()->text();
    }

    try {
        if (encryptToggleButton->isChecked() && encryptor && encryptor->isInitialized()) {
            string plaintext = text.toStdString();

            cout << "\n=== 加密过程 ===" << endl;
            cout << "原始消息: " << plaintext << endl;

            auto encrypted = encryptor->encryptAES(plaintext);

            cout << "加密成功，密文长度: " << encrypted.size() << " 字节" << endl;

            QString displayText = QString("[加密消息发送给 %1] %2").arg(selectedUser).arg(text);
            addMessageToHistory(currentUser, displayText);

            try {
                string decrypted = encryptor->decryptAES(encrypted);
                cout << "解密测试成功" << endl;

                QTimer::singleShot(1000, this, [this, selectedUser, decrypted]() {
                    QString reply = QString("收到你的加密消息: %1").arg(QString::fromStdString(decrypted));
                    addMessageToHistory(selectedUser.split(" ").first(), reply, false);
                    });
            }
            catch (const exception& e) {
                cerr << "解密测试失败: " << e.what() << endl;
            }

            updateStatus(QString("加密消息已发送给 %1 | 密文长度: %2 字节").arg(selectedUser).arg(encrypted.size()));
        }
        else {
            QString displayText = QString("[明文消息发送给 %1] %2").arg(selectedUser).arg(text);
            addMessageToHistory(currentUser, displayText);
            updateStatus(QString("明文消息已发送给 %1").arg(selectedUser));
        }

        messageInput->clear();
    }
    catch (const exception& e) {
        QMessageBox::critical(this, "加密错误",
            QString("消息处理失败: %1").arg(e.what()));
        updateStatus("错误: " + QString(e.what()));
    }
}

void ChatWindow::onSendFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "选择要发送的文件",
        QDir::homePath(), "所有文件 (*.*)");

    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        QString displayText = QString("[文件] %1 (%2)").arg(fileInfo.fileName())
            .arg(formatFileSize(fileInfo.size()));

        addMessageToHistory(currentUser, displayText);

        updateStatus(QString("准备发送文件: %1").arg(fileInfo.fileName()));

        QMessageBox::information(this, "文件发送",
            QString("准备发送文件: %1\n大小: %2\n\n文件加密功能开发中...")
            .arg(fileInfo.fileName())
            .arg(formatFileSize(fileInfo.size())));
    }
}

void ChatWindow::onShowKeyManager() {
    QMessageBox::information(this, "密钥管理",
        "密钥管理功能\n\n功能包括：\n• 生成RSA密钥对\n• 导入/导出密钥\n• 管理其他用户公钥\n• 查看密钥信息\n\n该功能正在开发中...");
}

void ChatWindow::onEncryptionToggled(bool checked) {
    if (checked) {
        encryptToggleButton->setText("加密模式");
        updateStatus("加密模式已启用");
    }
    else {
        encryptToggleButton->setText("明文模式");
        updateStatus("警告：明文模式，消息不加密！");
    }
}

void ChatWindow::onUserSelected(QListWidgetItem* item) {
    if (item) {
        QString user = item->text();
        updateStatus("已选择聊天对象: " + user);

        chatHistory->clear();
        chatHistory->append("正在与 " + user + " 对话");
        chatHistory->append("");
    }
}

void ChatWindow::testEncryption() {
    if (!encryptor || !encryptor->isInitialized()) {
        QMessageBox::warning(this, "测试失败", "加密模块未初始化");
        return;
    }

    try {
        string testMessage = "这是一条测试加密的消息！";

        cout << "\n=== 加密测试开始 ===" << endl;
        cout << "测试消息: " << testMessage << endl;

        auto encrypted = encryptor->encryptAES(testMessage);
        cout << "加密成功，密文长度: " << encrypted.size() << " 字节" << endl;

        string decrypted = encryptor->decryptAES(encrypted);
        cout << "解密成功: " << decrypted << endl;

        if (testMessage == decrypted) {
            cout << "加密/解密测试通过！" << endl;

            chatHistory->append("\n=== 加密测试 ===");
            chatHistory->append("测试消息: " + QString::fromStdString(testMessage));
            chatHistory->append("加密成功 ✓");
            chatHistory->append("解密成功 ✓");
            chatHistory->append("验证通过 ✓");

            updateStatus("加密测试通过！");
        }
        else {
            QMessageBox::warning(this, "测试失败", "加密解密验证失败！");
        }
    }
    catch (const exception& e) {
        QMessageBox::critical(this, "测试错误",
            QString("加密测试失败: %1").arg(e.what()));
    }
}

void ChatWindow::addMessageToHistory(const QString& sender, const QString& message, bool isMe) {
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString prefix = isMe ? "我" : sender;

    QString html = QString("<div style='margin: 5px 0;'>"
        "<span style='color: #666; font-size: 11px;'>[%1]</span> "
        "<span style='font-weight: bold; color: %2;'>%3: </span>"
        "<span style='color: #333;'>%4</span>"
        "</div>")
        .arg(time)
        .arg(isMe ? "#4CAF50" : "#2196F3")
        .arg(prefix)
        .arg(message.toHtmlEscaped());

    chatHistory->append(html);

    QTextCursor cursor = chatHistory->textCursor();
    cursor.movePosition(QTextCursor::End);
    chatHistory->setTextCursor(cursor);
}

QString ChatWindow::formatFileSize(qint64 bytes) {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (bytes >= GB) {
        return QString("%1 GB").arg(bytes / static_cast<double>(GB), 0, 'f', 1);
    }
    else if (bytes >= MB) {
        return QString("%1 MB").arg(bytes / static_cast<double>(MB), 0, 'f', 1);
    }
    else if (bytes >= KB) {
        return QString("%1 KB").arg(bytes / static_cast<double>(KB), 0, 'f', 1);
    }
    else {
        return QString("%1 B").arg(bytes);
    }
}

void ChatWindow::updateStatus(const QString& message) {
    statusLabel->setText(message);
    statusBar()->showMessage(message, 5000);
}