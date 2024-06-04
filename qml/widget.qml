import QtCore
import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import my.dircompare 1.0

Window {
    title: "BinCompare"
    visible: true
    color: "lightgrey"
    minimumWidth: 540
    minimumHeight: 400

    property string lastDirPath1: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
    property string lastDirPath2: StandardPaths.writableLocation(StandardPaths.DownloadLocation)

    ColumnLayout {
        anchors.fill: parent

        // DIRECTORY PATHS
        Text {
            text: "Enter paths or browse for directories"
            Layout.alignment: Qt.AlignLeft
            Layout.topMargin: 5
            Layout.leftMargin: 10
        }
        RowLayout {
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.bottomMargin: 5
            spacing: 5

            TextField {
                id: dirPathEdit1
                text: lastDirPath1
                leftPadding: 3
                topPadding: 3
                bottomPadding: 3

                Layout.fillWidth: true
            }
            Button {
                text: "Browse"
                leftPadding: 10
                rightPadding: 10
                onClicked: {
                    dirPathDialog1.open()
                }
            }
            TextField {
                id: dirPathEdit2
                text: lastDirPath2
                leftPadding: 3
                topPadding: 3
                bottomPadding: 3

                Layout.fillWidth: true
            }
            Button {
                text: "Browse"
                leftPadding: 10
                rightPadding: 10
                onClicked: {
                    dirPathDialog2.open()
                }
            }
        }

        // FILE TABLES
        RowLayout {
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.bottomMargin: 5
            spacing: 5
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 3

            TableView {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            TableView {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        // SEARCH SETTINGS
        RowLayout {
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.bottomMargin: 5
            spacing: 5

            Button {
                text: "Scan for duplicates"
                leftPadding: 10
                rightPadding: 10
                onClicked: {
                    false
                }
            }

            CheckBox {
                id: searchThreadedCheck
                text: "Multithreading"
                checked: false
            }

            Rectangle {
                color: "transparent"
                width: 80
                height: parent.height
            }

            Text {
                Layout.alignment: Qt.AlignRight
                text: "Filter by size (MB)"
            }

            TextField {
                Layout.alignment: Qt.AlignRight
                id: sizeFilterEdit
                text: "1000"
                Layout.fillWidth: true
                validator: IntValidator { bottom: 1; top: 1000000 }
            }
        }

        // SEARCH RESULT TABLE
        TableView {
            id: resultTable
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 4
        }

        // STATUS BAR
        RowLayout {
            spacing: 5
            Layout.alignment: Qt.AlignHCenter

            Text {
                id: statusText
                text: "Ready"
            }

            ProgressBar {
                id: progressBar
                from: 0
                to: 100
                value: 0
                width: 150
            }
        }
    }

    DirCompare {
        id: dirCompare
        onUpdateProgress: {
            progressBar.value = progress
        }
    }

    FolderDialog {
        id: dirPathDialog1
        title: "Select Directory"
        options: FolderDialog.ReadOnly
        currentFolder: lastDirPath1
        onAccepted: {
            dirPathEdit1.text = dirPathDialog1.selectedFolder
            lastDirPath1 = dirPathDialog1.selectedFolder

        }
    }

    FolderDialog {
        id: dirPathDialog2
        title: "Select Directory"
        options: FolderDialog.ReadOnly
        currentFolder: lastDirPath2
        onAccepted: {
            dirPathEdit2.text = dirPathDialog2.selectedFolder
            lastDirPath2 = dirPathDialog2.selectedFolder
        }
    }
}
