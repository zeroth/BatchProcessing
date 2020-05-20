import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.14
Item {
    id: root
    //    anchors.fill: parent
    property alias model: view.model
    ListView {
        id: view
        anchors.fill: parent
        model: model
        spacing: 5
        header: Rectangle{
            implicitWidth: ListView.view.width
            implicitHeight: 50
            color: "#279e61"
            RowLayout {

                anchors.fill: parent
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/4
                    color: "transparent"
                    Text {
                        anchors.centerIn: parent
                        text: "Job Type"
                    }
                }
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/4
                    color: "transparent"
                    Text{
                        anchors.centerIn: parent
                        text: "Status"
                    }
                }

                Rectangle{
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/4
                    color: "transparent"
                    Text{
                        anchors.centerIn: parent
                        text: "Start Time"
                    }
                }
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/4
                    color: "transparent"
                    Text {
                        anchors.centerIn: parent
                        text: "End Time"
                    }
                }
            }
        }

        delegate:listItem
    }

    Component {
        id: listItem

        Rectangle{
            property var availabelColors: ["#f6f6f6", "#e9e9e9"]
            width: ListView.view.width
            height: 65
            color: availabelColors[index %2]
            Rectangle {
            id:layoutContainer
            width: parent.width
            height: 30

            RowLayout {
                id:layout
                anchors.fill: parent

                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/4
                    Text {
                        anchors.centerIn: parent
                        text: jobid
                    }
                    color: "transparent"
                }
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/4

                    Text{
                        anchors.centerIn: parent
                        text: status
                    }
                    color: "transparent"
                }

                Rectangle{
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/4
                    Text{
                        anchors.centerIn: parent
                        text: startTime
                    }
                    color: "transparent"
                }
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/4
                    Text {
                        anchors.centerIn: parent
                        text: endTime
                    }
                    color: "transparent"
                }
            }
            }
            Rectangle {
                id:pathIndicator
                width: parent.width
                anchors.top:layoutContainer.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 20
                Text {
                    text: "C:\Code\Common\sample_data_non_deskew - Copy" // path
                    anchors.centerIn: parent
                }
            }

            ProgressBar {
                id:control
                anchors.top: pathIndicator.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                //               indeterminate: true
                from:0
                to:1.0
                value: 0.5//progress

                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 15
                    color: "#e6e6e6"
                    radius: 3
                    Text {
                        text: "Deskew"//process
                        font.pixelSize: 10
                        anchors.centerIn: parent
                    }
                }

                contentItem: Item {
                    implicitWidth: 200
                    implicitHeight: 13

                    Rectangle {
                        width: control.visualPosition * parent.width
                        height: parent.height
                        radius: 2
                        color: "#6617a819"
                    }
                }
            }
        }
    }

    /*ListModel {
        id:testModel
        ListElement {
            jobId: 1
            name: "Test 1"
            startTime: "now"
            endTime: "then"
        }
        ListElement {
            jobId: 2
            name: "Test 2"
            startTime: "now"
            endTime: "then"
        }
        ListElement {
            jobId: 3
            name: "Test 3"
            startTime: "now"
            endTime: "then"
        }
    }*/

}
