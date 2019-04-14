import * as React from "react";
import {default as TrackModel, TrackModelPluginListener} from "../models/track-model";
import {UUID} from "../utility";
import TrackPluginListItem from "./track-plugin-list-item";
import TrackPluginModel from "../models/track-plugin-model";
import Scrollable from "./scrollable";
import ActiveList from "./active-list";
import IconButton from "./icon-button";
import {faPlus} from "@fortawesome/fontawesome-free-solid";
import {ContextMenu, ContextMenuProvider} from "react-contexify";
import {PluginStoreListener} from "../stores/plugin-store";
import {ClickableComponent} from "./clickable-component";
import {PluginTypeInfo} from "../api/messages/plugin-manager-messages";
import ContextMenuContent from "./context-menu-content";
import {ArrayExtended} from "../array-extended";

export interface TrackPluginListProps {
    trackModel: TrackModel
    extendedList?: boolean;
}

export interface TrackPluginListState {
}

export default class TrackPluginList extends React.Component<TrackPluginListProps, TrackPluginListState> implements TrackModelPluginListener {

    protected pluginsAddedByUser: ArrayExtended<number> = new ArrayExtended();

    public componentWillMount() {
        this.props.trackModel.addPluginListener(this);
    }

    public componentWillUnmount() {
        this.props.trackModel.removePluginListener(this);
    }

    public pluginAdded(plugin: TrackPluginModel, position: number): void {
        if (this.pluginsAddedByUser.remove(plugin.getTypeId())) {
            window.WOAP.ServiceLocator.getPAGManager().openPluginModal(plugin);
        }
        this.forceUpdate();
    }

    public pluginRemoved(plugin: TrackPluginModel): void {
        this.forceUpdate();
    }

    public pluginMoved(pluginId: UUID, newPosition: number): void {
        this.forceUpdate();
    }

    public shouldComponentUpdate(props: TrackPluginListProps) {
        return this.props.trackModel !== props.trackModel
            || this.props.extendedList !== props.extendedList;
    }

    public componentWillReceiveProps(newProps: TrackPluginListProps) {
        if (this.props.trackModel !== newProps.trackModel) {
            this.props.trackModel.removePluginListener(this);
            newProps.trackModel.addPluginListener(this);
        }
    }

    public render() {
        if (this.props.extendedList) {
            return (
                <div className="trackPluginListWrapper">
                    <div className="extendedListHeader">
                        <div className="extendedListName">
                            Plugins
                        </div>
                        <ContextMenuProvider id="addPluginMenu" event="onClick" className="addButton">
                            <IconButton faIcon={faPlus}/>
                        </ContextMenuProvider>
                        <ContextMenu id="addPluginMenu">
                            <PluginTypeList trackModel={this.props.trackModel}
                                            onPluginAdd={(uid) => this.pluginsAddedByUser.add(uid)}
                            />
                        </ContextMenu>
                    </div>
                    <ActiveList
                        listId="trackPluginList"
                        listItemType="plugin"
                        className="trackPluginList draggable"
                        itemsFactory={() =>
                            this.props.trackModel.getPlugins().map((item: TrackPluginModel) => [item.getTrack().getId() + "," + item.getId(), item]) as [string, any]
                        }
                        itemDropped={(pluginIds, s, src, d, destinationIndex) => {
                            let pluginId = pluginIds.split(",")[1];
                            this.props.trackModel.movePlugin(pluginId, destinationIndex);
                        }}
                    >
                        {(pluginModel, dragHandleProps) => <TrackPluginListItem model={pluginModel}
                                                                                key={pluginModel.getId()}
                                                                                draggable={true}
                                                                                dragHandleProps={dragHandleProps}/>}
                    </ActiveList>
                </div>
            );
        } else {
            let menuId = this.props.trackModel.getId() + "pMenu";

            return (
                <Scrollable className="trackPluginList">
                    {this.props.trackModel.getPlugins().map((pluginModel: TrackPluginModel) =>
                        <TrackPluginListItem model={pluginModel} key={pluginModel.getId()}/>
                    )}
                    <ContextMenuProvider id={menuId} event="onDoubleClick" className="scrollablePlaceholder">
                        <div></div>
                    </ContextMenuProvider>
                    <ContextMenu id={menuId}>
                        <PluginTypeList trackModel={this.props.trackModel}
                                        onPluginAdd={(uid) => this.pluginsAddedByUser.add(uid)}
                        />
                    </ContextMenu>
                </Scrollable>
            );
        }
    }
}

interface PluginTypeListProps {
    trackModel: TrackModel;
    onPluginAdd?: (uid: number) => {};
}

interface PluginTypeListState {
    items: PluginTypeInfo[];
    loading: boolean;
}

class PluginTypeList extends React.Component<PluginTypeListProps, PluginTypeListState> implements PluginStoreListener {

    static defaultProps = {
        trackModel: null,
        onPluginAdd: () => {
        }
    };

    protected pluginTypeClickedBound;

    public constructor(props) {
        super(props);

        this.state = {
            items: [],
            loading: true
        };

        this.pluginTypeClickedBound = this.pluginTypeClicked.bind(this);
    }

    public componentWillMount() {
        window.WOAP.ServiceLocator.getStoresManager().PluginStore.addListener(this);
        this.updateList();
    }

    public componentWillUnmount(): void {
        window.WOAP.ServiceLocator.getStoresManager().PluginStore.removeListener(this);
    }

    public pluginTypesChanged(): void {
        this.updateList();
    }

    public updateList() {
        this.setState({loading: true});

        window.WOAP.ServiceLocator.getStoresManager().PluginStore.fetchPluginTypes().then((pluginTypes) => {
            this.setState({items: pluginTypes, loading: false});
        });
    }

    public pluginTypeClicked(uid: number) {
        window.WOAP.ContextMenuManager.dismissMenu();
        this.props.onPluginAdd(uid);
        this.props.trackModel.addPlugin(uid);
    }

    public render() {
        return (
            <ContextMenuContent className="pluginTypeList" showLoadingSpinner={this.state.loading}>
                {this.state.items.map((item) =>
                    <PluginTypeListItem key={item[0]}
                                        id={item[0]}
                                        name={item[1]}
                                        onClick={this.pluginTypeClickedBound}
                    />
                )}
            </ContextMenuContent>
        );
    }
}

interface PluginTypeListItemProps {
    id: number;
    name: string;
    onClick: (number) => void;
}

export class PluginTypeListItem extends ClickableComponent<PluginTypeListItemProps, {}> {

    public clicked(): void {
        this.props.onClick(this.props.id);
    }

    public shouldComponentUpdate(props: PluginTypeListItemProps) {
        return this.props.id !== props.id
            || this.props.name !== props.name;
    }

    public render() {
        return (
            <button {...this.getClickableEventProps()}
                    type="button"
                    className="pluginTypeListItem"
            >
                {this.props.name}
            </button>
        );
    }
}