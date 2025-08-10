import QtQuick
import QtQuick.Templates

TextField {
	id: control
	
	signal signalSetText(string str);
	
	function setTextName(str)
	{
		control.text = str;
	}
	function getText()
	{
		return control.text;
	}
	
	
	verticalAlignment: Text.AlignVCenter
	
	background: Rectangle {

		color: control.enabled ? "transparent" : "#353637"
		border.color: control.enabled ? "#21be2b" : "transparent"
	}

	Component.onCompleted: 
	{
		control.signalSetText.connect(control.setTextName);
	}
	
	Component.onDestruction: 
	{
		control.signalSetText.disconnect(control.setTextName);
	}
}
