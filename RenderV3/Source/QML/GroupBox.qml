import QtQuick
import QtQuick.Templates
//import QtQuick.Controls
//import QtQuick.Controls.Windows
//import QtQuick.Layouts
//import QtQuick.Controls.Basic

//GroupBox {
//	title: qsTr("Synchronize")
//	
//	background: Rectangle {
//		y: control.topPadding - control.bottomPadding
//		width: parent.width
//		height: parent.height - control.topPadding + control.bottomPadding
//		color: "transparent"
//		border.color: "#21be2b"
//		radius: 2
//	}
//	label: ColumnLayout {
//		anchors.fill: parent
//		CheckBox { text: qsTr("E-mail") }
//		CheckBox { text: qsTr("Calendar") }
//		CheckBox { text: qsTr("Contacts") }
//	}
//}

GroupBox {
	id: control
	title: qsTr("GroupBox")

	background: Rectangle {
		y: control.topPadding - control.bottomPadding
		width: parent.width
		height: parent.height - control.topPadding + control.bottomPadding
		color: "transparent"
		border.color: "#21be2b"
		radius: 2
	}

	label: Label {
		x: control.leftPadding
		width: control.availableWidth
		text: control.title
		color: "#21be2b"
		elide: Text.ElideRight
	}

	//Label {
	//	text: qsTr("Content goes here!")
	//}
}
