import * as React from "react";
import {
    default as TrackModel, TrackModelControlsListener
} from "../models/track-model";
import Panner from "./control-components/panner";

interface TrackPannerProps {
    model: TrackModel;
}

interface TrackPannerState {
    panLevel: number;
}

export default class TrackPanner extends React.Component<TrackPannerProps, TrackPannerState> implements TrackModelControlsListener {
    protected isBeingDragged: boolean = false;

    protected dragStartBound;
    protected dragEndBound;
    protected valueChangedCallbackBound;

    constructor(props) {
        super(props);

        this.state = {
            panLevel: this.props.model.getPan()
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
    }

    public panChanged(newValue: number): void {
        if (!this.isBeingDragged)
            this.setState({panLevel: newValue});
    }

    public muteChanged(newState: boolean): void {
    }

    public soloChanged(newState: boolean): void {
    }

    protected valueChangedCallback(newValue: number) {
        this.props.model.setPan(newValue);
        this.setState({panLevel: newValue});
    }

    public shouldComponentUpdate(props: TrackPannerProps, state: TrackPannerState) {
        return this.state.panLevel !== state.panLevel
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: TrackPannerProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeControlsListener(this);
            newProps.model.addControlsListener(this);
            this.setState({panLevel: newProps.model.getPan()});
        }
    }

    public dragStart() {
        this.isBeingDragged = true;
    }

    public dragEnd() {
        this.isBeingDragged = false;
        this.setState({panLevel: this.props.model.getPan()});
    }

    public render() {
        return (
            <div className="trackPanner">
                <Panner
                    value={this.state.panLevel}
                    valueChangedCallback={this.valueChangedCallbackBound}
                    minValue={-1}
                    maxValue={1}
                    isHorizontal={true}
                    dragEasing={(v) => v / 3}
                    dragStartCallback={this.dragStartBound}
                    dragEndCallback={this.dragEndBound}
                />
            </div>);
    }
}
