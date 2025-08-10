import QtQuick
import QtQuick.Templates

Button {
	id: control
	signal signalClicked();
	
	onClicked:
	{
		signalClicked();
	}
	background: Rectangle {
		//color: "transparent"
		color: control.down ? "#FF0000" : "#21be2b"
		Image {
			//width:  30
			//height: 30
			anchors.fill: parent
			source: "Images/close@3x.png"
			//anchors.horizontalCenter: clockFace.horizontalCenter
			//anchors.bottom: clockFace.verticalCenter
			
		}
	}
}
