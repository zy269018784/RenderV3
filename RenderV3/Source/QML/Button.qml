import QtQuick
import QtQuick.Templates

Button {
	id: control
	text: qsTr("")
	signal signalClicked();
	
	onClicked:
	{
		signalClicked();
	}
	
	contentItem: Text {
		text: control.text
		font: control.font
		opacity: enabled ? 1.0 : 0.3
		//color: control.down ? "#17a81a" : "#21be2b"
		color: control.down ? "#FF0000" : "#21be2b"
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		elide: Text.ElideRight
	}

	background: Rectangle {
		//implicitWidth:  30
		//implicitHeight: 30
		opacity: enabled ? 1 : 0.3
		//border.color: control.down ? "#17a81a" : "#21be2b"
		color: control.down ? "#FF0000" : "#21be2b"
		border.color: control.down ? "#FF0000" : "#21be2b"
		border.width: 1
		radius: 2
	}
}
