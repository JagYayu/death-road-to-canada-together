local Enum = require("tudov.Enum")

--- @class dr2c.CUIWidget
local CUIWidget = {}

--- @alias dr2c.WidgetType dr2c.CUIWidget.Type

CUIWidget.Type = Enum.sequence({
	Placeholder = 0,
	Button = 1,
})

CUIWidget.eventCWidgetNew = events:new(N_("CWidgetNew"), {
	"initialize",
	"overrides",
})

--- @param widgetType dr2c.WidgetType
--- @param parent Widget
--- @param args table?
--- @return Widget
function CUIWidget.new(widgetType, parent, args)
	--- @class Widget
	--- @field draw fun(renderer: Renderer)
	local widget = {
		type = widgetType,
		parent = parent,
	}

	--- @class dr2c.E.CWidgetNew
	local e = {
		widget = widget,
		args = args,
		key = widgetType,
	}

	events:invoke(CUIWidget.eventCWidgetNew, e, widgetType)

	if not widget.draw then
		error(("Invalid widget type %s: missing draw function"):format(widgetType), 2)
	end

	return widget
end

function CUIWidget.newButton(args)
	return CUIWidget.new(CUIWidget.Type.Button, args)
end

return CUIWidget
