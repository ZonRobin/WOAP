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
import TrackColorSelector from "./track-color-selector";
import TrackIOSelector from "./track-io-selector";
import TrackSendList from "./track-send-list";

interface TrackDetailProps {
    model: TrackModel;
}

interface TrackDetailState {

}

export default class TrackDetail extends React.Component<TrackDetailProps, TrackDetailState> implements TrackModelInfoListener {

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
    }

    public shouldComponentUpdate(props: TrackDetailProps) {
        return this.props.model !== props.model
            || this.props.model.getColor().getHex() === props.model.getColor().getHex();
    }

    public componentWillReceiveProps(newProps: TrackDetailProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeInfoListener(this);
            newProps.model.addInfoListener(this);
        }
    }

    public render() {
        const backgroundColor = new Color(0x808080).interpolate(this.props.model.getColor(), 0.1);

        return (
            <div className="trackDetail" style={{backgroundColor: backgroundColor.getHex()}}>
                <TrackNameLabel model={this.props.model} editable={true} maxFontSize={18} />
                <div className="trackIOColorButtons">
                    <TrackIOSelector model={this.props.model} input={true} />
                    <TrackColorSelector model={this.props.model} />
                    <TrackIOSelector model={this.props.model} input={false} />
                </div>
                <TrackPluginList trackModel={this.props.model} extendedList={true}/>
                <TrackSendList trackModel={this.props.model} extendedList={true}/>
                <TrackMuteButton model={this.props.model}/>
                <TrackSoloButton model={this.props.model}/>
                <TrackPanner model={this.props.model}/>
                <TrackGainSlider model={this.props.model}/>
            </div>
        );
    }
}
