import QtQuick          2.12
import QGroundControl         1.0
import QGroundControl.Controls 1.0
import QGroundControl.Palette  1.0
import QGroundControl.ScreenTools 1.0

// Merivus 自定义提示气泡组件，用于在鼠标悬停时显示简短说明内容。
Item {
    id: root

    // 提示文本内容，由外部组件赋值。
    property string text: ""

    // 提示框的最小宽度，避免提示文字内容过少时显示过窄。
    // 这里使用默认字体宽度的 8 倍作为基准，兼容不同 DPI 和分辨率。
    property real minimumWidth: ScreenTools.defaultFontPixelWidth * 8

    // 提示框最大宽度，外部组件可通过设置该属性限制提示气泡的横向尺寸。
    property real maximumWidth: 10000

    // 内部边距，用于提示文字与边框之间的空白。
    readonly property real _padding: 8

    // 组件宽度：先计算文本宽度加左右边距，再与最小宽度比较，最后不超过最大宽度。
    width: Math.min(Math.max(minimumWidth, tipText.contentWidth + (_padding * 2)), maximumWidth)

    // 组件高度：根据文本的隐式高度加上下边距计算。
    height: tipText.implicitHeight + (_padding * 2)

    // 当文本为空时隐藏提示框。
    visible: text.length > 0

    // 将提示框置于最顶层，避免被其他元素覆盖。
    z: 100000

    // 使用 QGC 全局调色板，保证提示框颜色与主题一致。
    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    // 背景矩形：带圆角、半透明背景和边框。
    Rectangle {
        anchors.fill: parent
        radius: 6
        color: Qt.rgba(qgcPal.windowShade.r, qgcPal.windowShade.g, qgcPal.windowShade.b, 0.98)
        border.color: Qt.rgba(qgcPal.text.r, qgcPal.text.g, qgcPal.text.b, 0.24)
        border.width: 1
    }

    // 文本标签：显示提示内容，超长文本右侧省略。
    QGCLabel {
        id: tipText
        x: root._padding
        y: root._padding
        text: root.text
        color: qgcPal.text
        font.pixelSize: 11
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
}