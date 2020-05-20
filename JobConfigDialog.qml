import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import BatchProcessor 1.0

Dialog {
    id: root
    property alias inputDirPath: txInputDirPath.text
    property alias outputDirPath: txOutputDirPath.text
    property alias xyPixel: tfXYPixel.text
    property alias zStep: tfZStep.text
    property alias deskewAngle: tfAngle.text
    property alias channelInfo: tfChannelSelector.text
    property alias deskew: chkDeskew.checked
    property alias decon: chkDecon.checked
    property alias bleachCorrect: chkBleachCorrect.checked
    property alias hyperStack: chkHyperStack.checked
    property alias timePointSelector: tfTimeSelector.text
    property bool isList: false
    property var dirList;

    title: "Job Configuration"
    standardButtons: Dialog.Ok | Dialog.Cancel
    Settings {
        id: jobConfigSetting
        fileName: settingPath
        category: "JobConfig"
        property alias xyPixel: root.xyPixel
        property alias zStep: root.zStep
        property alias deskewAngle: root.deskewAngle
        property alias deskew: root.deskew
        property alias hyperStack: root.hyperStack
        property alias timePointSelector: root.timePointSelector
        property alias channelInfo: root.channelInfo
    }

    function openDialog(inputPath) {
        dirList = inputPath;
        inputDirPath = inputPath.join(",");
//        console.log(dirList);
//        if(inputPath.length > 1){
//            isList = true
//            inputDirPath = inputPath.join(",");
//        } else {
//            isList = false
//            inputDirPath = inputPath[0]
//            dirList = inputPath[0]
//        }

        this.open();
    }
    onAccepted:  {
        // pass the config to producer
        var jobObj = {
            "inputDir" : root.dirList,
            "outputDir" : root.outputDirPath,
            "deskewAngle" : root.deskewAngle,
            "xyPixel" : root.xyPixel,
            "zStep" :  root.zStep,
            "channelInfo" : root.channelInfo.split(","),
            "deskew": root.deskew,
            "decon": root.decon,
            "bleachCorrect": root.bleachCorrect,
            "hyperStack": root.hyperStack,
            "timePointSelector" : root.timePointSelector
        };

        if(!producer.createJob(jobObj)) {
            // show some status/ notification
        }
    }
    onActionChosen:  {
        // impliment the validation logic for this dialog
        action.accepted = true
    }

    GridLayout {
        id: gridLayout
        anchors.fill: parent
        columnSpacing: 8
        rowSpacing: 8
        rows: 8
        columns: 9
        // row 1
        Label {
            id: labelInput
            text: qsTr("Input Directory")
            Layout.columnSpan: 3
            leftPadding: 5
        }

        TextField {
            id: txInputDirPath
            Layout.minimumWidth: 140
            Layout.fillWidth: true
            Layout.columnSpan: 3
            placeholderText: qsTr("Path")
        }

        Button {
            id:btnInput
            Layout.columnSpan: 3
            text: "Select"
            onClicked: {
                inputDialog.open()
            }
        }

        // row 2

        Label {
            id: labelOutput
            text: qsTr("Output Directory")
            Layout.columnSpan: 3
            leftPadding: 5
        }

        TextField {
            id: txOutputDirPath
            Layout.minimumWidth: 140
            Layout.fillWidth: true
            Layout.columnSpan: 3
            placeholderText: qsTr("Path")
        }

        Button {
            id:btnOutput
            Layout.columnSpan: 3
            text: "Select"
            onClicked: {
                outputDialog.open()
            }
        }

        // row 3
        Label {
            id: labelXYPixel
            text: qsTr("XY Pixel Size um")
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignCenter
        }
        Label {
            id: labelZStep
            text: qsTr("Z Pixel Size um")
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignCenter
        }
        Label {
            id: labelAngle
            text: qsTr("Deskew Angle")
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignCenter
        }

        // row 4
        TextField {
            id: tfXYPixel
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignCenter
            property int decimals: 4
            validator: DoubleValidator {
                bottom: 0
                top:  100.0
            }

        }

        TextField {
            id: tfZStep
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignCenter
            property int decimals: 4
            validator: DoubleValidator {
                bottom: 0
                top:  100.0
            }
        }

        TextField {
            id: tfAngle
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignCenter
            property int decimals: 4
            validator: DoubleValidator {
                bottom: 0
                top:  360.0
            }
        }

        // row 5
        CheckBox {
            id: chkDeskew
            Layout.columnSpan: 2
            checked: true
            text: qsTr("Deskew")
        }

        CheckBox {
            id: chkBleachCorrect
            Layout.columnSpan: 2
            checked: false
            text: qsTr("Bleach Correct")
            enabled: false
        }
        CheckBox {
            id: chkDecon
            Layout.columnSpan: 2
            checked: false
            text: qsTr("Deconvolution")
            enabled: false
        }
        CheckBox {
            id: chkHyperStack
            Layout.columnSpan: 3
            checked: false
            text: qsTr("Hyper Stack")
        }
        // row 6

        Label {
            id: labelTimeSelector
            text: qsTr("Time point selector")
            Layout.columnSpan: 4
            Layout.alignment: Qt.AlignCenter
        }

        TextField {
            id:tfTimeSelector
            Layout.columnSpan: 5
            placeholderText: qsTr("Ex. _T, _stack")
        }

        // row 7

        Label {
            id: labelChannelSelector
            text: qsTr("Channel Selector string")
            Layout.columnSpan: 4
            Layout.alignment: Qt.AlignCenter
        }

        TextField {
            id:tfChannelSelector
            Layout.columnSpan: 5
            placeholderText: qsTr("Ex. _488, _560 or CamA, CamB")
        }
    }

    FileDialog {
        id: inputDialog
        selectExisting: true
        selectMultiple: false
        selectFolder: true
        onAccepted: {
            txInputDirPath.text = inputDialog.folder;
            dirList = [inputDialog.folder];
        }
    }

    FileDialog {
        id: outputDialog
        selectExisting: true
        selectMultiple: false
        selectFolder: true
        onAccepted: {
            txOutputDirPath.text = outputDialog.folder;
        }
    }

}
