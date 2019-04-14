import * as React from "react";
import {
    default as TrackModel, TrackModelControlsListener
} from "../models/track-model";
import LinearSlider from "./control-components/linear-slider";
import TrackLevelIndicator from "./track-level-indicator";
import VerticalDbScale from "./control-components/vertical-db-scale";

interface TrackGainSliderProps {
    model: TrackModel;
}

interface TrackGainSliderState {
    gainLevel: number;
}

export default class TrackGainSlider extends React.Component<TrackGainSliderProps, TrackGainSliderState> implements TrackModelControlsListener {
    protected isBeingDragged: boolean = false;

    static defaultProps = {
        model: null
    };

    protected dragStartBound;
    protected dragEndBound;
    protected valueChangedCallbackBound;

    constructor(props) {
        super(props);

        this.state = {
            gainLevel: this.props.model.getGain()
        };

        this.dragStartBound = this.dragStart.bind(this);
        this.dragEndBound = this.dragEnd.bind(this);
        this.valueChangedCallbackBound = this.valueChangedCallback.bind(this);
    }

    public componentWillMount() {
        this.props.model.addControlsListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeControlsListener(this);
    }

    public gainChanged(newValue: number): void {
        if (!this.isBeingDragged)
            this.setState({gainLevel: newValue});
    }

    public panChanged(newValue: number): void {
    }

    public muteChanged(newState: boolean): void {
    }

    public soloChanged(newState: boolean): void {
    }

    protected valueChangedCallback(newValue: number) {
        this.props.model.setGain(newValue);
        this.setState({gainLevel: newValue});
    }

    public shouldComponentUpdate(props: TrackGainSliderProps, state: TrackGainSliderState) {
        return this.state.gainLevel !== state.gainLevel
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: TrackGainSliderProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeControlsListener(this);
            newProps.model.addControlsListener(this);
            this.setState({gainLevel: newProps.model.getGain()});
        }
    }

    public dragStart() {
        this.isBeingDragged = true;
    }

    public dragEnd() {
        this.isBeingDragged = false;
        this.setState({gainLevel: this.props.model.getGain()});
    }

    protected thumb = <svg width="30" height="40" shapeRendering="crispEdges">
        <rect x="0" y="0" width="100%" height="100%" rx="3" ry="3" opacity="0.85"
              style={{fill: "rgb(40, 40, 40)"}}/>

        <rect x="5%" y="50%" width="90%" height="2" style={{fill: "rgb(230, 230, 230)"}}/>

        <rect x="5%" y="35%" width="90%" height="1" style={{fill: "rgb(100, 100, 100)"}}/>
        <rect x="5%" y="65%" width="90%" height="1" style={{fill: "rgb(100, 100, 100)"}}/>

        <rect x="5%" y="20%" width="90%" height="1" style={{fill: "rgb(100, 100, 100)"}}/>
        <rect x="5%" y="80%" width="90%" height="1" style={{fill: "rgb(100, 100, 100)"}}/>
    </svg>;

    public render() {
        return (
            <div className="gainSlider trackGainSlider">
                <TrackLevelIndicator model={this.props.model}/>
                <VerticalDbScale width={30}
                                 margin={20}
                                 minValue={-145}
                                 maxValue={10}
                                 skewFactor={3}
                />
                <LinearSlider
                    value={this.state.gainLevel}
                    minValue={-145}
                    maxValue={10}
                    skewFactor={3}
                    valueChangedCallback={this.valueChangedCallbackBound}
                    dragStartCallback={this.dragStartBound}
                    dragEndCallback={this.dragEndBound}
                >
                    {this.thumb}
                </LinearSlider>
            </div>);
    }
}
