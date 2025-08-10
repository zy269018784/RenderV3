import QtQuick
import QtQuick.Templates

ListView {
	id: control
	ListModel {
       id: myModel
    }
	signal signalClear();
	signal signalAppend(string str);
	
	//onSignalClear:
	//{
	//	clear();
	//}
	
    model: myModel
	
	delegate: Text {
         text: name + ":\t" + number
    }
	
	Component.onCompleted: 
	{
		control.signalAppend.connect(myModel.append);
		control.signalClear.connect(myModel.clear);
	}
	
	Component.onDestruction: 
	{
		control.signalAppend.disconnect(MyModel.append);
		control.signalClear.disconnect(MyModel.clear);
	}
}

