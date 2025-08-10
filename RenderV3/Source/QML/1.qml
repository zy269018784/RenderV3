// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick

Rectangle {
    id: root
    property color rectColor: "red"

    Rectangle {
        property int d: 100
        id: square
        width: d
        height: d
        anchors.centerIn: parent
        color: root.rectColor
        //NumberAnimation on rotation { from: 0; to: 360; duration: 2000; loops: Animation.Infinite; }
    }

    Text {
        id: text
        anchors.centerIn: parent
        text: "Qt Quick running in a widget\nGraphicsInfo.api says: "
    }

}
