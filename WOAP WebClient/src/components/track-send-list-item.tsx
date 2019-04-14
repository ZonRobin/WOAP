import * as React from "react";
import NameLabel from "./name-label";
import ColorHelper from "../helpers/color/color-helper";
import Color from "../helpers/color/color";
import DragHandle from "./drag-handle";
import {DragHandleProps} from "react-beautiful-dnd";
import {TrackModelInfoListener} from "../models/track-model";
import {default as TrackSendModel, TrackSendModelListener} from "../models/track-send-model";
import {ContextMenu, ContextMenuProvider} from "react-contexify";
import TrackSendDetail from "./track-send-detail";

interface TrackSendListItemProps {
    model: TrackSendModel;
    draggable?: boolean;
    dragHandleProps?: DragHandleProps;
}

interface TrackSendListItemState {

}

export default class TrackSendListItem extends React.Component<TrackSendListItemProps, TrackSendListItemState> implements TrackSendModelListener, TrackModelInfoListener {
    protected willBeDeleted = false;

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

    public gainChanged(newGain: number): void {
    }

    public panChanged(newPan: number): void {
    }

    public audioSourcePositionChanged(newPosition: number): void {
    }

    public colorChanged(newColor: Color): void {
        this.forceUpdate();
    }

    public sendWillBeDeleted(): void {
        this.willBeDeleted = true;
        this.forceUpdate();
    }

    public trackWillBeDeleted(): void {
    }

    public shouldComponentUpdate(props: TrackSendListItemProps) {
        return this.props.model !== props.model
            || this.props.draggable !== props.draggable;
    }

    public componentWillReceiveProps(newProps: TrackSendListItemProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeListener(this);
            this.props.model.getTrack().removeInfoListener(this);
            newProps.model.addListener(this);
            newProps.model.getTrack().addInfoListener(this);
        }
    }

    public render() {
        let contextId = this.props.model.getId() + "dtl";
        let sendColor = this.props.model.getColor();

        return (
            <div className={"trackSendListItem" + (this.willBeDeleted ? " willBeDeleted" : "")}
                 style={{backgroundColor: ColorHelper.getBrighterColor(sendColor, 0.3).getHex()}}
            >
                <ContextMenuProvider id={contextId} event="onClick">
                    <NameLabel text={this.props.model.getName()}
                               useDarkTextColor={ColorHelper.getPerceivedBrightness(sendColor) > 0.5}
                               fitText={!this.props.draggable}
                    />

                    {this.props.draggable ? <DragHandle dragHandleProps={this.props.dragHandleProps}/> : []}
                </ContextMenuProvider>
                <ContextMenu id={contextId}>
                    <TrackSendDetail model={this.props.model}/>
                </ContextMenu>
            </div>);
    }
}