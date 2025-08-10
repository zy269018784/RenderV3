import QtQuick
import QtQuick.Templates

Label 
{
	id: root
	signal signalSetText(string str);
	function setText(str)
	{
		root.text = str;
	}
	text: qsTr("Label")
	font.family: "Microsoft YaHei UI"
	font.pointSize: 9
	font.bold: false
	color: "#000000"
	Component.onCompleted:
	{
		root.signalSetText.connect(root.setText);
	}
}

