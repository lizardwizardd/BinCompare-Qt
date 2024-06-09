import QtCore
import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import Qt.labs.qmlmodels 1.0
import my.tableupdater 1.0

Window {
    title: "BinCompare"
    visible: true
    color: "lightgrey"
    minimumWidth: 540
    minimumHeight: 400

    property string lastDirPath1: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
    property string lastDirPath2: StandardPaths.writableLocation(StandardPaths.DownloadLocation)

    Component.onCompleted: {
        fileTable1.model = tableUpdater.updateFileTable(dirPathEdit1.text)
        fileTable2.model = tableUpdater.updateFileTable(dirPathEdit2.text)
        statusText.text = "Ready"
    }

    ColumnLayout {
        anchors.fill: parent

        // DIRECTORY PATHS
        Text {
            text: "Browse for directories to compare"
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
                readOnly: true

                Layout.fillWidth: true
            }
            Button {
                text: "Browse"
                leftPadding: 10
                rightPadding: 10
                onClicked: {
                    statusText.text = "Browsing for directory"
                    dirPathDialog1.open()
                }
            }
            TextField {
                id: dirPathEdit2
                text: lastDirPath2
                leftPadding: 3
                topPadding: 3
                bottomPadding: 3
                readOnly: true

                Layout.fillWidth: true
            }
            Button {
                text: "Browse"
                leftPadding: 10
                rightPadding: 10
                onClicked: {
                    statusText.text = "Browsing for directory"
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
                id: fileTable1
                columnSpacing: 1
                rowSpacing: 1
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                Layout.fillWidth: true
                Layout.fillHeight: true

                columnWidthProvider: function(column) {
                    if (column === 0) {
                        return fileTable1.width * 0.8 - 1
                    }
                    if (column === 1) {
                        return fileTable1.width * 0.2 - 1
                    }
                    return 100;
                }

                rowHeightProvider: function(row) {
                    return 23
                }

                delegate: Rectangle {
                    implicitWidth: 150
                    implicitHeight: 50
                    Text {
                        text: display
                        elide: Text.ElideRight
                        width: parent.width
                        padding: 3
                    }
                }
            }
            TableView  {
                id: fileTable2
                columnSpacing: 1
                rowSpacing: 1
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                Layout.fillWidth: true
                Layout.fillHeight: true


                columnWidthProvider: function(column) {
                    if (column === 0) {
                        return fileTable1.width * 0.8 - 1
                    }
                    if (column === 1) {
                        return fileTable1.width * 0.2 - 1
                    }
                    return 100;
                }

                rowHeightProvider: function(row) {
                    return 23
                }

                delegate: Rectangle {
                    implicitWidth: 150
                    implicitHeight: 50
                    Text {
                        text: display
                        elide: Text.ElideRight
                        width: parent.width
                        padding: 3
                    }
                }
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
                    resultTable.model = tableUpdater.updateDuplicatesTable
                            (dirPathEdit1.text, dirPathEdit2.text, sizeFilterEdit.text, searchThreadedCheck.checked)
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
            columnSpacing: 1
            rowSpacing: 1
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 4
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            columnWidthProvider: function(column) {
                if (column === 0 || column === 1) {
                    return resultTable.width * 0.45 - 1
                } else if (column === 2) {
                    return resultTable.width * 0.1 - 1
                }
                return 100;
            }

            rowHeightProvider: function(row) {
                return 25
            }

            delegate: Rectangle {
                implicitWidth: 150
                implicitHeight: 50
                Text {
                    text: display
                    elide: Text.ElideRight
                    width: parent.width
                    padding: 3
                }
            }
        }

        // STATUS BAR
        RowLayout {
            spacing: 5
            Layout.alignment: Qt.AlignRight

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

    TableUpdater {
        id: tableUpdater
        onUpdateStatusBar: {
            statusText.text = message
        }
        onUpdateProgressBar: {
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
            fileTable1.model = tableUpdater.updateFileTable(dirPathEdit1.text)
            statusText.text = "Directory changed. Ready"
        }
        onRejected: {
            statusText.text = "Browse dialog closed. Ready"
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
            fileTable2.model = tableUpdater.updateFileTable(dirPathEdit2.text)
            statusText.text = "Directory changed. Ready"
        }
        onRejected: {
            statusText.text = "Browse dialog closed. Ready"
        }
    }
}
