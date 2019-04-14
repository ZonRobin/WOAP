import * as React from "react";
import TrackSendModel, {TrackSendModelListener} from "../models/track-send-model";
import {default as TrackPluginModel} from "../models/track-plugin-model";
import {default as TrackModel, TrackModelPluginListener} from "../models/track-model";
import Color from "../helpers/color/color";
import {Utility, UUID} from "../utility";
import Scrollable from "./scrollable";
import ButtonList from "./button-list";
import {ClickableComponent} from "./clickable-component";

interface TrackSendAudioSourcePositionSelectorProps {
    model: TrackSendModel;
}

interface TrackSendAudioSourcePositionSelectorState {
    items: Array<[number, string]>;
    listHidden: boolean;
}

export default class TrackSendAudioSourcePositionSelector extends ClickableComponent<TrackSendAudioSourcePositionSelectorProps, TrackSendAudioSourcePositionSelectorState>
    implements TrackSendModelListener, TrackModelPluginListener {

    static defaultProps = {
        model: null
    };

    protected itemClickedBound;
    protected offMouseUpBound;

    public constructor(props) {
        super(props);

        this.state = {
            items: [],
            listHidden: true
        };

        this.itemClickedBound = this.itemClicked.bind(this);
        this.offMouseUpBound = this.offMouseUp.bind(this);
    }

    public componentWillMount() {
        this.props.model.addListener(this);
        this.props.model.getTrack().addPluginListener(this);
        this.updateList();
    }

    public componentWillUnmount() {
        this.props.model.removeListener(this);
        this.props.model.getTrack().removePluginListener(this);
    }

    public shouldComponentUpdate(props: TrackSendAudioSourcePositionSelectorProps, state: TrackSendAudioSourcePositionSelectorState) {
        return this.props.model !== props.model
            || this.state.listHidden !== state.listHidden;
    }

    public colorChanged(newColor: Color): void {
    }

    public nameChanged(newName: string): void {
    }

    public gainChanged(newGain: number): void {
    }

    public panChanged(newPan: number): void {
    }

    public audioSourcePositionChanged(newPosition: number): void {
        this.updateList();
    }

    public sendWillBeDeleted(): void {
    }

    public pluginAdded(plugin: TrackPluginModel, position: number): void {
        this.updateList();
    }

    public pluginRemoved(plugin: TrackPluginModel): void {
        this.updateList();
    }

    public pluginMoved(pluginId: UUID, oldPosition: number, newPosition: number): void {
        this.updateList();
    }

    public clicked(): void {
        if (this.state.listHidden) {
            this.showList();
        } else {
            this.hideList();
        }
    }

    public hideList() {
        if (this.state.listHidden)
            return;

        this.setState({listHidden: true});
    }

    public showList() {
        if (!this.state.listHidden)
            return;

        this.setState({listHidden: false});
    }

    public itemClicked(positionIndex: number) {
        this.props.model.setAudioSourcePosition(positionIndex);
        this.hideList();
    }

    public offMouseUp(event) {
        if (this.state.listHidden || window.WOAP.MouseWatcher.wasTouchEndLately())
            return;

        if (!window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown()) {
            let element: HTMLElement = event.target;
            while (element !== null) {
                if (typeof element.className === "string" && element.className.indexOf("audioSourcePositionList") !== -1) {
                    return;
                }

                element = element.parentElement;
            }

            this.hideList();
        }
    }

    public componentWillReceiveProps(newProps: TrackSendAudioSourcePositionSelectorProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeListener(this);
            this.props.model.getTrack().removePluginListener(this);
            newProps.model.addListener(this);
            newProps.model.getTrack().addPluginListener(this);
        }
    }

    public componentDidUpdate(prevProps, prevState) {
    if (!this.state.listHidden && prevState.listHidden) {
            Utility.addEventListener(window, "touchstart", this.offMouseUpBound);
            Utility.addEventListener(window, "mousedown", this.offMouseUpBound);
        } else if (this.state.listHidden && !prevState.listHidden) {
            Utility.removeEventListener(window, "mousedown", this.offMouseUpBound);
            Utility.removeEventListener(window, "touchstart", this.offMouseUpBound);
        }
    }

    public updateList() {
        let newItems = [];

        newItems.push([TrackModel.AudioSourcePosition.PrePlugins, "Before plugins"]);
        let i = 1;
        this.props.model.getTrack().getPlugins().forEach((plugin) =>
            newItems.push([i++, "After " + plugin.getName()])
        );
        newItems.push([TrackModel.AudioSourcePosition.PreFader, "Before fader"]);
        newItems.push([TrackModel.AudioSourcePosition.PostFader, "After fader"]);

        this.setState({items: newItems});
    }

    public getLabel(): string {
        let position = this.props.model.getAudioSourcePosition();

        switch (position) {
            case TrackModel.AudioSourcePosition.PrePlugins:
                return "Before plugins";
            case TrackModel.AudioSourcePosition.PreFader:
                return "Before fader";
            case TrackModel.AudioSourcePosition.PostFader:
                return "After fader";
            default: {
                let plugins = this.props.model.getTrack().getPlugins();
                if (position > plugins.getLength()) {
                    return "Unknown";
                } else {
                    return "After " + plugins.get(position - 1).getName();
                }
            }
        }
    }

    public render() {
        return (
            <div className="audioSourcePositionSelect">
                <div {...this.getClickableEventProps()}>{this.getLabel()}</div>
                <Scrollable
                    className={this.state.listHidden ? "hidden audioSourcePositionList" : "audioSourcePositionList"}
                >
                    <ButtonList buttons={this.state.items} onItemClick={this.itemClickedBound}/>
                </Scrollable>
            </div>
        );
    }
}
