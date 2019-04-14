import * as React from "react";
import {
    default as TrackModel,
    TrackModelControlsListener,
} from "../models/track-model";
import {ClickableComponent} from "./clickable-component";

interface TrackButtonProps {
    model: TrackModel;
}

interface TrackButtonState {
    active: boolean;
}

export class TrackMuteButton extends ClickableComponent<TrackButtonProps, TrackButtonState> implements TrackModelControlsListener {
    constructor(props) {
        super(props);

        this.state = {
            active: this.props.model.getMute()
        };
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
    }

    public muteChanged(newState: boolean): void {
        this.setState({active: newState});
    }

    public soloChanged(newState: boolean): void {
    }

    public clicked(): void {
        if (!window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown())
            this.props.model.setMute(!this.state.active);
    }

    public shouldComponentUpdate(props: TrackButtonProps, state: TrackButtonState) {
        return this.state.active !== state.active
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: TrackButtonProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeControlsListener(this);
            newProps.model.addControlsListener(this);
            this.setState({active: newProps.model.getMute()});
        }
    }

    public render() {
        let active = this.props.model.getMute();

        return (
            <button {...this.getClickableEventProps()}
                    className={"trackMuteButton" + (active ? " active" : "")}
                    type="button"
            >
                Mute
            </button>);
    }
}


export class TrackSoloButton extends ClickableComponent<TrackButtonProps, TrackButtonState> implements TrackModelControlsListener {
    constructor(props) {
        super(props);

        this.state = {
            active: this.props.model.getSolo()
        };
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
    }

    public muteChanged(newState: boolean): void {
    }

    public soloChanged(newState: boolean): void {
        this.setState({active: newState});
    }

    public clicked(): void {
        this.props.model.setSolo(!this.state.active);
    }

    public shouldComponentUpdate(props: TrackButtonProps, state: TrackButtonState) {
        return this.state.active !== state.active
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: TrackButtonProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeControlsListener(this);
            newProps.model.addControlsListener(this);
            this.setState({active: newProps.model.getSolo()});
        }
    }

    public render() {
        let active = this.props.model.getSolo();

        return (
            <button {...this.getClickableEventProps()}
                    className={"trackSoloButton" + (active ? " active" : "")}
                    type="button"
            >
                Solo
            </button>);
    }
}
