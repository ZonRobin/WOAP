import * as React from "react";
import TrackSendModel from "../models/track-send-model";
import TrackSendNameLabel from "./track-send-name-label";
import TrackSendPanner from "./track-send-panner";
import TrackSendGainSlider from "./track-send-gain-slider";
import TrackSendAudioSourcePositionSelector from "./track-send-audio-source-position-selector";
import ContextMenuContent from "./context-menu-content";
import Color from "../helpers/color/color";

interface TrackSendDetailProps {
    model: TrackSendModel;
}

interface TrackSendDetailState {

}

export default class TrackSendDetail extends React.Component<TrackSendDetailProps, TrackSendDetailState> {

    public shouldComponentUpdate(props: TrackSendDetailProps) {
        return this.props.model !== props.model;
    }

    public render() {
        const backgroundColor = new Color(0x6A6A6A).interpolate(this.props.model.getColor(), 0.1);

        return (
            <ContextMenuContent className="trackSendDetail" backgroundColor={backgroundColor.getHex()}>
                <TrackSendNameLabel model={this.props.model} editable={true} maxFontSize={18}/>
                <TrackSendAudioSourcePositionSelector model={this.props.model}/>
                <TrackSendPanner model={this.props.model}/>
                <TrackSendGainSlider model={this.props.model}/>
            </ContextMenuContent>
        );
    }
}
