import QtQuick
import QtQuick.Templates

Button {
	id: control
	property color previousColor;
	
	signal signalClicked();
	signal signalSetColor(color c);
	
	function setColor(c)
	{
		//bg.color = c;
		previousColor = c;
	}
	
	onClicked:
	{
		signalClicked();
	}
	text: qsTr("")
	
	background: Rectangle {
		id: bg
		opacity: enabled ? 1 : 0.3
		//border.color: control.down ? "#17a81a" : "#21be2b"
		//color: control.down ? "#17a81a" : "#21be2b"
		//border.color: control.down ? "#17a81a" : "#21be2b"
		
		color: 			control.down ? "#FF0000" : previousColor
		border.color: 	control.down ? "#FF0000" : previousColor
		border.width: 1
		radius: 2
	}
	
	Component.onCompleted: 
	{
		previousColor = "#000000";
		control.signalSetColor.connect(control.setColor);
	}
	
	Component.onDestruction: 
	{
		control.signalSetColor.disconnect(control.setColor);
	}
}
