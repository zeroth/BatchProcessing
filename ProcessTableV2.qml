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
        delegate: listItem

        header:  Rectangle {
            implicitWidth: ListView.view.width
            implicitHeight: (view.count >0)? 50 : ListView.view.height
            color: "#279e61"
            Rectangle {
                id: emptyBox
                visible: !(view.count >0)
                anchors.fill:parent
                Text {
                    anchors.centerIn: parent
                    text: "Drag and drop the directory you want to process"
                }
            }

            RowLayout {
                visible: (view.count >0)
                anchors.fill: parent
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width*(70/100)
                    color: "transparent"
                    Text {
                        anchors.centerIn: parent
                        text: "Job"
                    }
                }
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width*(30/100)
                    color: "transparent"
                    Text{
                        anchors.centerIn: parent
                        text: "Status"
                    }
                }
            }
        }
    }

    Component {
        id: emptyList
        Rectangle {
            anchors.fill:parent
            Text {
                Text {
                    text: "Drag and Drop the directory you want to process"
                    anchors.centerIn: parent;
                }
            }
        }
    }

    Component {
        id: listItem

        Rectangle{
            id: rootDelegate
            property var availabelColors: ["#f6f6f6", "#e9e9e9"]
            width: ListView.view.width
            height: 50
            color: availabelColors[index %2]
            MouseArea {
                anchors.fill: parent
                onClicked:  {
                    timeContainer.visible = !timeContainer.visible
                    rootDelegate.height =  timeContainer.visible ? 70 : 50
                    timeContainer.height =  timeContainer.visible ? 20 : 0
                }
            }

            Rectangle {
                id:layoutContainer
                width: parent.width
                height: 35
                color: "transparent"
                RowLayout {
                    id:layout
                    anchors.fill: parent
//                    anchors.leftMargin: 10

                    Rectangle {

                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width *(70/100)
                        color: "transparent"

                        Text {
                            text: path
                            anchors.centerIn: parent
                        }
                    }
                    Rectangle {
                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width * (30/100)
                        color: "transparent"
                        Text{
                            text: status
                            anchors.centerIn: parent
                        }

                    }

                }
            }

            Rectangle {
                id:timeContainer
                visible: false
                height: 0
                anchors.top: layoutContainer.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                color: "transparent"
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    Rectangle{
                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width/3
                        Row {
                            spacing: 10
                            height: parent.height
                            Text{

                                text: "Start Time :"
                                font.italic: true
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text{

                                text: startTime
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }


                        color: "transparent"
                    }
                    Rectangle {
                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width/3
                        Row {
                            height: parent.height
                            spacing: 10
                            Text {
                                text: "End Time :"
                                font.italic: true
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                text: endTime
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                        color: "transparent"
                    }
                }

            }

            ProgressBar {
                id:control
                anchors.top: timeContainer.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                //               indeterminate: true
                from:0
                to:1.0
                value: progress

                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 15
                    color: "#e6e6e6"
                    radius: 3
                    Text {
                        text: process
                        font.pixelSize: 10
//                        font.bold: true
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
