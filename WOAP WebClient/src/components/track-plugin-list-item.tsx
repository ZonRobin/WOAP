import * as React from "react";
import {default as PluginModel, TrackPluginModelListener} from "../models/track-plugin-model";
import NameLabel from "./name-label";
import ColorHelper from "../helpers/color/color-helper";
import {ClickableComponent} from "./clickable-component";
import Color from "../helpers/color/color";
import DragHandle from "./drag-handle";
import {DragHandleProps} from "react-beautiful-dnd";
import {TrackModelInfoListener} from "../models/track-model";

interface TrackPluginListItemProps {
    model: PluginModel;
    draggable?: boolean;
    dragHandleProps?: DragHandleProps;
}

interface TrackPluginListItemState {

}

export default class TrackPluginListItem extends ClickableComponent<TrackPluginListItemProps, TrackPluginListItemState> implements TrackPluginModelListener, TrackModelInfoListener {
    protected boundMouseUp;
    protected willBeDeleted = false;

    public constructor(props) {
        super(props);
        this.boundMouseUp = this.mouseUpCallback.bind(this);
    }

    public componentWillMount() {
        this.props.model.addListener(this);
        this.props.model.getTrack().addInfoListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeListener(this);
        this.props.model.getTrack().removeInfoListener(this);
    }

    public nameChanged(newName: string): void {
        this.forceUpdate();
    }

    public bypassChanged(newState: boolean): void {
        this.forceUpdate();
    }

    public activeChanged(newState: boolean): void {
        this.forceUpdate();
    }

    public colorChanged(newColor: Color): void {
        this.forceUpdate();
    }

    public pluginWillBeDeleted(): void {
        this.willBeDeleted = true;
        this.forceUpdate();
    }

    public trackWillBeDeleted(): void {
    }

    public isPluginEnabled(): boolean {
        return !this.props.model.getBypass() && this.props.model.getActive();
    }

    public clicked(): void {
        if (!window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown())
            window.WOAP.ServiceLocator.getPAGManager().openPluginModal(this.props.model);
    }

    public shouldComponentUpdate(props: TrackPluginListItemProps) {
        return this.props.model !== props.model
            || this.props.draggable !== props.draggable;
    }

    public componentWillReceiveProps(newProps: TrackPluginListItemProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeListener(this);
            this.props.model.getTrack().removeInfoListener(this);
            newProps.model.addListener(this);
            newProps.model.getTrack().addInfoListener(this);
        }
    }

    protected static pluginInactiveColor: Color = new Color(0xA0A0A0);

    public render() {
        let trackColor = this.props.model.getTrack().getColor();
        let pluginEnabled = this.isPluginEnabled();

        let backgroundColor = pluginEnabled ? ColorHelper.getBrighterColor(trackColor, 0.3) : TrackPluginListItem.pluginInactiveColor;
        let darkText = pluginEnabled ? ColorHelper.getPerceivedBrightness(trackColor) > 0.5 : true;

        return (
            <div className={"trackPluginListItem" + (this.willBeDeleted ? " willBeDeleted" : "")}
                 {...this.getClickableEventProps()}
                 style={{backgroundColor: backgroundColor.getHex()}}
            >
                <NameLabel text={this.props.model.getName()}
                           useDarkTextColor={darkText}
                           fitText={!this.props.draggable}
                />

                {this.props.draggable ? <DragHandle dragHandleProps={this.props.dragHandleProps}/> : []}
            </div>);
    }
}