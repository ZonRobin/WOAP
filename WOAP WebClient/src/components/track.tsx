import * as React from "react";
import {
    default as TrackModel,
    TrackModelInfoListener,
} from "../models/track-model";
import Color from "../helpers/color/color";
import TrackPluginList from "./track-plugin-list";
import TrackNameLabel from "./track-name-label";
import {TrackMuteButton, TrackSoloButton} from "./track-control-buttons";
import TrackGainSlider from "./track-gain-slider";
import TrackPanner from "./track-panner";
import DragHandle from "./drag-handle";
import {DragHandleProps} from "react-beautiful-dnd";
import ColorHelper from "../helpers/color/color-helper";

interface TrackProps {
    model: TrackModel;
    dragHandleProps: DragHandleProps;
    selected?: boolean;
}

interface TrackState {

}

export default class Track extends React.Component<TrackProps, TrackState> implements TrackModelInfoListener {

    static defaultProps = {
        model: null,
        dragHandleProps: {},
        selected: false
    };

    protected willBeDeleted = false;
    protected boundOnDragClick;

    public constructor(props) {
        super(props);
        this.boundOnDragClick = this.dragOnClick.bind(this);
    }

    public componentWillMount() {
        this.props.model.addInfoListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeInfoListener(this);
    }

    public nameChanged(newName: string): void {
    }

    public colorChanged(newColor: Color): void {
        this.forceUpdate();
    }

    public trackWillBeDeleted(): void {
        this.willBeDeleted = true;
        this.forceUpdate();
    }

    public shouldComponentUpdate(props: TrackProps) {
        return this.props.selected !== props.selected
            || this.props.model.getColor().getHex() === props.model.getColor().getHex()
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: TrackProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeInfoListener(this);
            newProps.model.addInfoListener(this);
        }
    }

    public dragOnClick() {
        window.WOAP.ServiceLocator.getStoresManager().TrackStore.selectTrack(this.props.model);
    }

    public render() {
        const backgroundColor = new Color(0x808080).interpolate(this.props.model.getColor(), 0.1);
        let className = "track";

        if (this.willBeDeleted) {
            className += " willBeDeleted";
        } else if (this.props.selected) {
            className += " selected";
        }

        return (

            <div className={className}
                 style={{backgroundColor: this.props.selected ? ColorHelper.getBrighterColor(backgroundColor).getHex() : backgroundColor.getHex()}}>
                <TrackPluginList trackModel={this.props.model}/>
                <TrackNameLabel model={this.props.model}/>
                <TrackMuteButton model={this.props.model}/>
                <TrackSoloButton model={this.props.model}/>
                <TrackPanner model={this.props.model}/>
                <TrackGainSlider model={this.props.model}/>
                <DragHandle dragHandleProps={this.props.dragHandleProps} onClick={this.boundOnDragClick}/>
            </div>
        );
    }
}
