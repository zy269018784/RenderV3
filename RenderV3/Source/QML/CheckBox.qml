import QtQuick
import QtQuick.Templates

CheckBox {
	id: control
	
	signal signalSetText(string str);
	signal signalChecked(bool str);
	function setText(str)
	{
		control.text = str;
	}

	function setColor(c)
	{
		controlText.color = c;
	}

	onClicked:
	{
		signalChecked(control.checked);
	}
	
	//text: qsTr("ACES Tone Mapping")
	checked: false

	indicator: Rectangle {
		implicitWidth: 26
		implicitHeight: 26
		x: control.leftPadding
		y: parent.height / 2 - height / 2
		radius: 3
		border.color: control.down ? "#17a81a" : "#21be2b"

		Rectangle {
			width: 14
			height: 14
			x: 6
			y: 6
			radius: 2
			color: control.down ? "#17a81a" : "#21be2b"
			visible: control.checked
		}
	}

	contentItem: Text {
		id: controlText
		text: control.text
		font: control.font
		opacity: enabled ? 1.0 : 0.3
		color: control.down ? "#17a81a" : "#21be2b"
		verticalAlignment: Text.AlignVCenter
		leftPadding: control.indicator.width + control.spacing + 5
	}
	
	Component.onCompleted: 
	{
		control.signalSetText.connect(control.setText);
	}
	
	Component.onDestruction: 
	{
		control.signalSetText.disconnect(control.setText);
	}
}
