import * as React from "react";
import {default as TrackModel} from "../models/track-model";
import {ContextMenu, ContextMenuProvider} from "react-contexify";
import {UUID} from "../utility";
import {ClickableComponent} from "./clickable-component";
import Color from "../helpers/color/color";
import {TrackStoreConnectionsListener} from "../stores/track-store";
import {AvailableMixerNodeInfo} from "../api/messages/audio-core-messages";
import ContextMenuContent from "./context-menu-content";

interface TrackIOSelectorProps {
    model: TrackModel;
    input: boolean;
}

interface TrackIOSelectorState {
}

export default class TrackIOSelector extends ClickableComponent<TrackIOSelectorProps, TrackIOSelectorState> {

    static defaultProps = {
        model: null,
        input: true
    };

    public shouldComponentUpdate(props: TrackIOSelectorProps, state: TrackIOSelectorState) {
        return this.props.model !== props.model
            || this.props.input !== props.input;
    }

    public clicked(): void {
    }

    public render() {
        return (
            <div className={this.props.input ? "inputSelectButton" : "outputSelectButton"}>
                <ContextMenuProvider id={this.props.input ? "inputNodeList" : "outputNodeList"} event="onClick">
                    <button type="button" {...this.getClickableEventProps()}>
                        {this.props.input ? "Input" : "Output"}
                    </button>
                </ContextMenuProvider>
                <ContextMenu id={this.props.input ? "inputNodeList" : "outputNodeList"}>
                    <IONodeList trackModel={this.props.model} input={this.props.input}/>
                </ContextMenu>
            </div>
        );
    }
}

interface IONodeListProps {
    trackModel: TrackModel;
    input: boolean;
}

interface IONodeListState {
    items: AvailableMixerNodeInfo[];
    loading: boolean
}

class IONodeList extends React.Component<IONodeListProps, IONodeListState> implements TrackStoreConnectionsListener {

    static defaultProps = {
        trackModel: null,
        input: true
    };

    protected ioNodeClickedBound;

    public constructor(props) {
        super(props);

        this.state = {
            items: [],
            loading: true
        };

        this.ioNodeClickedBound = this.ioNodeClicked.bind(this);
        window.WOAP.ServiceLocator.getStoresManager().TrackStore.addConnectionsListener(this);
    }

    public connectionsChanged(): void {
        this.updateList();
    }

    public componentWillMount() {
        this.updateList();
    }

    public componentWillUnmount(): void {
        window.WOAP.ServiceLocator.getStoresManager().TrackStore.removeConnectionsListener(this);
    }

    public updateList() {
        this.setState({loading: true});

        if (this.props.input) {
            this.props.trackModel.fetchAvailableInputSources().then((sources) => {
                this.setState({items: sources, loading: false});
            });
        } else {
            this.props.trackModel.fetchAvailableOutputReceivers().then((sources) => {
                this.setState({items: sources, loading: false});
            });
        }
    }

    public componentWillReceiveProps(newProps: IONodeListProps): void {
        if (this.props.trackModel !== newProps.trackModel) {
            this.updateList();
        }
    }

    public ioNodeClicked(id: UUID, active: boolean) {
        this.setState({loading: true});

        let source = this.props.input ? id : this.props.trackModel.getId();
        let receiver = this.props.input ? this.props.trackModel.getId() : id;

        if (!active)
            window.WOAP.ServiceLocator.getStoresManager().TrackStore.connect(source, receiver);
        else
            window.WOAP.ServiceLocator.getStoresManager().TrackStore.removeConnection(source, receiver);
    }

    public render() {
        return (
            <ContextMenuContent className="ioNodeList" showLoadingSpinner={this.state.loading}>
                {this.state.items.map((item: [UUID, string, string, boolean]) =>
                    <IONodeListItem key={item[0]} id={item[0]} name={item[1]} color={item[2]} active={item[3]}
                                    onClick={this.ioNodeClickedBound}/>
                )}
            </ContextMenuContent>
        );
    }
}

interface IONodeListItemProps {
    id: UUID;
    name: string;
    color: string;
    active: boolean;
    onClick: (UUID, boolean) => void;
}

export class IONodeListItem extends ClickableComponent<IONodeListItemProps, {}> {

    public clicked(): void {
        this.props.onClick(this.props.id, this.props.active);
    }

    public shouldComponentUpdate(props: IONodeListItemProps) {
        return this.props.id !== props.id
            || this.props.name !== props.name
            || this.props.color !== props.color
            || this.props.active !== props.active;
    }

    public render() {
        let backgroundColor;
        if (this.props.active)
            backgroundColor = new Color("#1FBA46");
        else
            backgroundColor = new Color("#AAAAAA").interpolate(new Color("#" + this.props.color), 0.1);

        return (
            <button {...this.getClickableEventProps()}
                    type="button"
                    style={{backgroundColor: backgroundColor}}
                    className="ioNodeListItem"
            >
                {this.props.name}
            </button>
        );
    }
}