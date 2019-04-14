import * as React from "react";
import {default as TrackModel, TrackModelPluginListener, TrackModelSendListener} from "../models/track-model";
import {UUID} from "../utility";
import Scrollable from "./scrollable";
import ActiveList from "./active-list";
import IconButton from "./icon-button";
import {faPlus} from "@fortawesome/fontawesome-free-solid";
import {ContextMenu, ContextMenuProvider} from "react-contexify";
import {ClickableComponent} from "./clickable-component";
import TrackSendModel from "../models/track-send-model";
import TrackSendListItem from "./track-send-list-item";
import {default as TrackPluginModel} from "../models/track-plugin-model";
import ContextMenuContent from "./context-menu-content";

export interface TrackSendListProps {
    trackModel: TrackModel
    extendedList?: boolean;
}

export interface TrackSendListState {
}

export default class TrackSendList extends React.Component<TrackSendListProps, TrackSendListState> implements TrackModelSendListener {

    public componentWillMount() {
        this.props.trackModel.addSendListener(this);
    }

    public componentWillUnmount() {
        this.props.trackModel.removeSendListener(this);
    }

    public sendAdded(send: TrackSendModel, position: number): void {
        this.forceUpdate();
    }

    public sendRemoved(send: TrackSendModel): void {
        this.forceUpdate();
    }

    public sendMoved(sendId: UUID, oldPosition: number, newPosition: number): void {
        this.forceUpdate();
    }

    public shouldComponentUpdate(props: TrackSendListProps) {
        return this.props.trackModel !== props.trackModel
            || this.props.extendedList !== props.extendedList;
    }

    public componentWillReceiveProps(newProps: TrackSendListProps) {
        if (this.props.trackModel !== newProps.trackModel) {
            this.props.trackModel.removeSendListener(this);
            newProps.trackModel.addSendListener(this);
        }
    }

    public render() {
        if (this.props.extendedList) {
            return (
                <div className="trackSendListWrapper">
                    <div className="extendedListHeader">
                        <div className="extendedListName">
                            Sends
                        </div>
                        <ContextMenuProvider id="addSendMenu" event="onClick" className="addButton">
                            <IconButton faIcon={faPlus}/>
                        </ContextMenuProvider>
                        <ContextMenu id="addSendMenu">
                            <SendTypeList trackModel={this.props.trackModel}/>
                        </ContextMenu>
                    </div>
                    <ActiveList
                        listId="trackSendList"
                        listItemType="send"
                        className="trackSendList draggable"
                        itemsFactory={() =>
                            this.props.trackModel.getSends().map((item: TrackSendModel) => [item.getTrack().getId() + "," + item.getId(), item]) as [string, any]
                        }
                        itemDropped={(sendIds, s, src, d, destinationIndex) => {
                            let sendId = sendIds.split(",")[1];
                            this.props.trackModel.moveSend(sendId, destinationIndex);
                        }}
                    >
                        {(sendModel, dragHandleProps) => <TrackSendListItem model={sendModel}
                                                                            key={sendModel.getId()}
                                                                            draggable={true}
                                                                            dragHandleProps={dragHandleProps}/>}
                    </ActiveList>
                </div>
            );
        } else {
            return (
                <Scrollable className="trackSendList">
                    {this.props.trackModel.getSends().map((sendModel: TrackSendModel) =>
                        <TrackSendListItem model={sendModel} key={sendModel.getId()}/>
                    )}
                </Scrollable>
            );
        }
    }
}

interface SendTypeListProps {
    trackModel: TrackModel;
}

interface SendTypeListState {
    items: Array<[number, string]>;
}

class SendTypeList extends React.Component<SendTypeListProps, SendTypeListState> implements TrackModelPluginListener {
    static defaultProps = {
        trackModel: null
    };

    protected sendTypeClickedBound;

    public constructor(props) {
        super(props);

        this.state = {
            items: []
        };

        this.sendTypeClickedBound = this.sendTypeClicked.bind(this);
    }

    public componentWillMount() {
        this.props.trackModel.addPluginListener(this);
        this.updateList();
    }

    public componentWillUnmount(): void {
        this.props.trackModel.removePluginListener(this);
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

    public updateList() {
        let newItems = [];

        newItems.push([TrackModel.AudioSourcePosition.PrePlugins, "Before plugins"]);
        let i = 1;
        this.props.trackModel.getPlugins().forEach((plugin) =>
            newItems.push([i++, "After " + plugin.getName()])
        );
        newItems.push([TrackModel.AudioSourcePosition.PreFader, "Before fader"]);
        newItems.push([TrackModel.AudioSourcePosition.PostFader, "After fader"]);

        this.setState({items: newItems});
    }

    public sendTypeClicked(audioSourcePosition: number) {
        window.WOAP.ContextMenuManager.dismissMenu();
        this.props.trackModel.addSend(audioSourcePosition);
    }

    public render() {
        return (
            <ContextMenuContent className="sendTypeList">
                {this.state.items.map((item) =>
                    <SendTypeListItem key={item[0]} audioSourcePosition={item[0]} name={item[1]}
                                      onClick={this.sendTypeClickedBound}/>
                )}
            </ContextMenuContent>
        );
    }
}

interface SendTypeListItemProps {
    audioSourcePosition: number;
    name: string;
    onClick: (number) => void;
}

export class SendTypeListItem extends ClickableComponent<SendTypeListItemProps, {}> {

    public clicked(): void {
        this.props.onClick(this.props.audioSourcePosition);
    }

    public shouldComponentUpdate(props: SendTypeListItemProps) {
        return this.props.audioSourcePosition !== props.audioSourcePosition
            || this.props.name !== props.name;
    }

    public render() {
        return (
            <button {...this.getClickableEventProps()}
                    type="button"
                    className="sendTypeListItem"
            >
                {this.props.name}
            </button>
        );
    }
}