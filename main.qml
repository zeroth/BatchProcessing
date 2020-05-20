import QtQuick 2.14
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

Rectangle {
//    visible: true
    anchors.fill: parent

    ToolBar{
        id: toolBar
        leftPadding: 8
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        RowLayout {
            ToolButton {
                text: "+ New Job"
                onClicked: {
                    jobConfigDialog.openDialog("")
                }
            }
            ToolSeparator{
                height: parent.height
            }

            ToolButton {
                text: "+ New HyperStack"
                onClicked: {
                    console.log("New HyperStack")
                }
            }
        }

    }

    ProcessTableV2 {
        anchors.top: toolBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: producer.mode()
        DropArea {
            id: drop
            enabled: true
            anchors.fill: parent
            keys: ["text/uri-list"]
            Rectangle {
                anchors.fill: parent
                color: Qt.rgba(0, 1, 0, 0.5)
                visible: parent.containsDrag
                Text {
                    text: "Drop Zone"
                    anchors.centerIn: parent
                    font.pointSize: 40
                    font.bold: true
                }
            }
            onDropped:{
                jobConfigDialog.openDialog(drop.text.trim().split("\n"));
            }
        }
    }

    JobConfigDialog {
        id: jobConfigDialog
    }
}
