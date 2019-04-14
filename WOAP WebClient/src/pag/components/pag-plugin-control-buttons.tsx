import * as React from "react";
import {default as TrackPluginModel, TrackPluginModelListener} from "../../models/track-plugin-model";
import {ClickableComponent} from "../../components/clickable-component";

interface PAGControlButtonProps {
    model: TrackPluginModel;
}

interface PAGControlButtonState {
    active: boolean;
}

export class PAGPluginActiveButton extends ClickableComponent<PAGControlButtonProps, PAGControlButtonState> implements TrackPluginModelListener {
    constructor(props) {
        super(props);

        this.state = {
            active: this.props.model.getActive()
        };
    }

    public componentWillMount() {
        this.props.model.addListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeListener(this);
    }

    public nameChanged(newName: string): void {
    }

    public bypassChanged(newState: boolean): void {
    }

    public activeChanged(newState: boolean): void {
        this.setState({active: newState});
    }

    public pluginWillBeDeleted(): void {
    }

    public clicked(): void {
        if (!window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown())
            this.props.model.setActive(!this.state.active);
    }

    public shouldComponentUpdate(props: PAGControlButtonProps, state: PAGControlButtonState) {
        return this.state.active !== state.active
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: PAGControlButtonProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeListener(this);
            newProps.model.addListener(this);
            this.setState({active: newProps.model.getActive()});
        }
    }

    public render() {
        let active = this.props.model.getActive();

        return (
            <button {...this.getClickableEventProps()}
                    className={"pagPluginActiveButton" + (active ? " active" : "")}
                    type="button"
            >
                Active
            </button>);
    }
}


export class PAGPluginBypassButton extends ClickableComponent<PAGControlButtonProps, PAGControlButtonState> implements TrackPluginModelListener {
    constructor(props) {
        super(props);

        this.state = {
            active: this.props.model.getBypass()
        };
    }

    public componentWillMount() {
        this.props.model.addListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeListener(this);
    }

    public nameChanged(newName: string): void {
    }

    public bypassChanged(newState: boolean): void {
        this.setState({active: newState});
    }

    public activeChanged(newState: boolean): void {
    }

    public pluginWillBeDeleted(): void {
    }

    public clicked(): void {
        if (!window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown())
            this.props.model.setBypass(!this.state.active);
    }

    public shouldComponentUpdate(props: PAGControlButtonProps, state: PAGControlButtonState) {
        return this.state.active !== state.active
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: PAGControlButtonProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeListener(this);
            newProps.model.addListener(this);
            this.setState({active: newProps.model.getBypass()});
        }
    }

    public render() {
        let active = this.props.model.getBypass();

        return (
            <button {...this.getClickableEventProps()}
                    className={"pagPluginBypassButton" + (active ? " active" : "")}
                    type="button"
            >
                Bypass
            </button>);
    }
}
