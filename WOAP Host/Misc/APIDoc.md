# API #

## APICore messages ##

APICore handles connecting clients and communication.

**ApiCoreHeartbeat**  
Type: outcoming – sent each 5 seconds to all clients  
Address: /heartbeat

**ApiCoreGetStatus**  
Type: incoming  
Address: /getStatus

**ApiCoreReady** - response for ApiCoreGetStatus  
Type: outcoming  
Address: /ready

**ApiCoreError**  
Type: outcoming  
Address: /error

**ApiCoreMessageRejected** - response for ApiCoreGetStatus  
Type: outcoming  
Address: /messageRejected  
Arguments:  

1. int32 (reason) [1001 – invalid message, 1002 – unauthorized user, 1003 - parsing error]
2. int32 (rejectedMessageId) id of message (-1 if it has no id)
3. string (debug info)

**ApiCoreGetServerInfo**  
Type: incoming query  
Address: /getServerInfo

**ApiCoreServerInfo**  
Type: outcoming query  
Address: /serverInfo  
Arguments:

1. string (deviceVersion)
2. string (deviceId)
3. string (processId)
4. int32 (anonymousLoginEnabled) bool value
5. int32 (userAuthenticated) bool value

**ApiCoreLogin**  
Type: incoming query  
Address: /login  
Arguments:
 
1. string (password)

**ApiCoreLoginResponse** - response for ApiCoreLogin  
Type: outcoming response  
Address: /login  
Arguments:   

1. int32 (responseCode) [1000 - Ok, 1001 - Wrong credentials]
2. string (logTag)

**ApiCoreAuthenticate**  
Type: incoming query  
Address: /authenticate  
Arguments:

1. string (logTag) Token got when logged in

**ApiCoreAuthenticationResponse** - response for ApiCoreAuthenticate  
Type: outcoming response  
Address: /authenticationResponse  
Arguments:   

1. int32 (responseCode) [1000 - Ok, 1001 - wrong logtag]

## Audio Core messages ##

AudioCore is the root instance of the whole system. It owns all tracks, input/output proxies, handles callbacks, connections and more.  
  
**AudioCoreAddTrack**  
Type: incoming  
Address: /audioCore/addTrack  
Arguments:

1. int32 (trackType) [1 - AudioTrack, 2 - FxTrack, 3 - MixBusTrack, 4 - OutputBusTrack]
2. int32 (trackPosition) [-1 for end of list]
  
**AudioCoreTrackAdded**  
Type: outcoming  
Address: /audioCore/trackAdded  
Arguments:
   
1. string (trackId)
2. int32 (trackPosition) [-1 for end of list]

**AudioCoreRemoveTrack**  
Type: incoming  
Address: /audioCore/removeTrack  
Arguments:   

1. string (trackId)
  
**AudioCoreTrackRemoved**  
Type: outcoming  
Address: /audioCore/trackRemoved  
Arguments:   

1. string (trackId)

**AudioCoreMoveTrack** - moves track in list of tracks  
Type: incoming  
Address: /audioCore/moveTrack  
Arguments:

1. string (trackId)
2. int32 (newTrackPosition) [-1 for end of list]

**AudioCoreTrackMoved**  
Type: outcoming  
Address: /audioCore/trackMoved  
Arguments:   

1. string (trackId)
2. int32 (oldTrackPosition)
3. int32 (newTrackPosition) [-1 for end of list]

**AudioCoreConnectNodes** - connects two mixer nodes (tracks, input/output proxies)  
Type: incoming  
Address: /audioCore/connectNodes  
Arguments:   

1. string (sourceNodeId)
2. string (receiverNodeId)

**AudioCoreRemoveNodesConnection**  
Type: incoming  
Address: /audioCore/removeNodesConnection  
Arguments:   

1. string (sourceNodeId)
2. string (receiverNodeId)

**AudioCoreConnectionsChanged**  
Type: outcoming  
Address: /audioCore/connectionsChanged

**AudioCoreGetTracksList**  
Type: incoming query  
Address: /audioCore/getTracksList

**AudioCoreTracksList**  
Type: outcoming response  
Address: /audioCore/tracksList  
Arguments:   

1. string (trackIds) JSON array of strings (array of track ids)

**AudioCoreGetInputProxiesList**  
Type: incoming query  
Address: /audioCore/getInputProxiesList

**AudioCoreInputProxiesList**  
Type: outcoming response  
Address: /audioCore/inputProxiesList  
Arguments:   

1. string (proxyIds) JSON array of strings (array of proxy ids)

**AudioCoreInputProxyAdded**  
Type: outcoming  
Address: /audioCore/inputProxyAdded  
Arguments:  

1. string (proxyId)
2. string (proxyName)

**AudioCoreInputProxyRemoved**  
Type: outcoming  
Address: /audioCore/inputProxyRemoved  
Arguments:  

1. string (proxyId)

**AudioCoreGetOutputProxiesList**  
Type: incoming query  
Address: /audioCore/getOutputProxiesList

**AudioCoreOutputProxiesList**  
Type: outcoming response  
Address: /audioCore/outputProxiesList  
Arguments:   

1. string (proxyIds) JSON array of strings (array of proxy ids)

**AudioCoreOutputProxyAdded**  
Type: outcoming  
Address: /audioCore/outputProxyAdded  
Arguments:  

1. string (proxyId)
2. string (proxyName)

**AudioCoreOutputProxyRemoved**  
Type: outcoming  
Address: /audioCore/outputProxyRemoved  
Arguments:  

1. string (proxyId)

**AudioCoreGetAvailableInputSourcesList**  
Type: incoming query  
Address: /audioCore/getAvailableInputSourcesList  
Arguments:   

1. string (receiverNodeId)

**AudioCoreAvailableInputSourcesList** - list of other nodes a node can connect to  
Type: outcoming response  
Address: /audioCore/availableInputSourcesList  
Arguments:   

1. string (sources) JSON array of nodes [UUID, name, color, active]

**AudioCoreGetAvailableOutputReceiversList**  
Type: incoming query  
Address: /audioCore/getAvailableOutputReceiversList  
Arguments:   

1. string (sourceNodeId)

**AudioCoreAvailableOutputReceiversList** - list of other nodes a node can connect to  
Type: outcoming response  
Address: /audioCore/availableOutputReceiversList  
Arguments:   

1. string (receivers) JSON array of nodes [UUID, name, color, active] 

**AudioCoreGetGlobalAudioLatency**  
Type: incoming query  
Address: /audioCore/getGlobalAudioLatency
  
**AudioCoreGlobalAudioLatency**  
Type: outcoming response  
Address: /audioCore/globalAudioLatency  
Arguments:   

1. int32 (latencySamples)
2. float (latencyMs)
  
**AudioCoreGlobalAudioLatencyChanged**  
Type: outcoming  
Address: /audioCore/globalAudioLatencyChanged  
Arguments:   

1. int32 (latencySamples)
2. float (latencyMs)
  
**AudioCoreSignalLevels**  
Type: outcoming  
Address: /audioCore/signalLevels  
Arguments:   

1. int32 (numTracks)
2. blob (tracksSignalLevels) 16bit PCM data tracks one-by-one [leftChan, rightChan]
3. blob (soloBusSignalLevels) 16bit PCM data [leftChan, rightChan]
4. blob (trackClipData) bit mask, each track has one bit indicating clip

**AudioCoreAddListener** - registers a client to listen for events  
Type: incoming  
Address: /audioCore/addListener  
Registers for:

- AudioCoreTrackAdded
- AudioCoreTrackRemoved
- AudioCoreTrackMoved
- AudioCoreConnectionsChanged
- AudioCoreInputProxyAdded
- AudioCoreInputProxyRemoved
- AudioCoreOutputProxyAdded
- AudioCoreOutputProxyRemoved
- AudioCoreGlobalAudioLatencyChanged
  
  
**AudioCoreRemoveListener**  
Type: incoming  
Address: /audioCore/removeListener
  
**AudioCoreAddSignalLevelListener** - registers a client to listen for streaming of tracks signal levels  
Type: incoming  
Address: /audioCore/addSignalLevelListener  
Arguments:   

1. int32 (refreshIntervalHz) Interval in which stream data (from 1 to 30 Hz)  

Registers for:

- AudioCoreSignalLevels

**AudioCoreRemoveSignalLevelListener**  
Type: incoming  
Address: /audioCore/removeSignalLevelListener

**AudioCoreAudioData** – message containing audio samples  
Type: outcoming  
Address: /audioCore/audioData  
Arguments:   

1. int32 (numSamples) number of samples in each channel
2. int32 (numChannels)
3. blob (audioSamples) 16bit PCM audio sample data – channels one after another

## Track messages ##

**TrackGetDetail**  
Type: incoming query  
Address: /track/{trackId}/getDetail

**TrackDetail**  
Type: outcoming response  
Address: /track/{trackId}/detail  
Arguments:   

1. int32 (trackType) [1 - AudioTrack, 2 - FxTrack, 3 - MixBusTrack, 4 - OutputBusTrack]
2. float32 (gain)
3. float32 (pan)
4. int32 (mute) bool value
5. int32 (solo) bool value
6. int32 (recordArm) bool value
7. string (name)
8. string (colour) hex value
9. string (pluginIds) JSON array of plugin IDs in the chain
10. string (sendIds) JSON array of send IDs

**TrackSetGain**  
Type: incoming  
Address: /track/{trackId}/setGain  
Arguments:   

1. float32 (gain)

**TrackGainChanged**  
Type: outcoming  
Address: /track/{trackId}/gainChanged  
Arguments:   

1. float32 (gain)

**TrackSetPan**  
Type: incoming  
Address: /track/{trackId}/setPan  
Arguments:   

1. float32 (pan)
  
**TrackPanChanged**  
Type: outcoming  
Address: /track/{trackId}/panChanged  
Arguments:   

1. float32 (pan)
  
**TrackSetMute**  
Type: incoming  
Address: /track/{trackId}/setMute  
Arguments:   

1. int32 (mute) bool value
  
**TrackMuteChanged**  
Type: outcoming  
Address: /track/{trackId}/muteChanged  
Arguments:
   
1. int32 (mute) bool value

**TrackSetSolo**  
Type: incoming  
Address: /track/{trackId}/setSolo  
Arguments:   

1. int32 (solo) bool value
  
**TrackSoloChanged**  
Type: outcoming  
Address: /track/{trackId}/soloChanged  
Arguments:   

1. int32 (solo) bool value

**TrackSetRecordArm**  
Type: incoming  
Address: /track/{trackId}/setRecordArm  
Arguments:
   
1. int32 (recordArm) bool value
  
**TrackRecordArmChanged**  
Type: outcoming  
Address: /track/{trackId}/recordArmChanged  
Arguments:   

1. int32 (recordArm) bool value

**TrackSetName**  
Type: incoming  
Address: /track/{trackId}/setName  
Arguments:   

1. string (name)
  
**TrackNameChanged**  
Type: outcoming  
Address: /track/{trackId}/nameChanged  
Arguments:   

1. string (name)

**TrackSetColour**  
Type: incoming  
Address: /track/{trackId}/setColour  
Arguments:   

1. string (colour) hex value
  
**TrackColourChanged**  
Type: outcoming  
Address: /track/{trackId}/colourChanged  
Arguments:   

1. string (colour) hex value

**TrackAddSendNode**  
Type: incoming  
Address: /track/{trackId}/addSendNode  
Arguments:   

1. int32 (audioSourcePosition) [0 - before plugins, 1 - 999 after nth plugin, 1000 - after plugins, 1001 - after fader]
2. int32 (sendPosition) (-1 end of list)

**TrackSendAdded**  
Type: outcoming  
Address: /track/{trackId}/sendAdded  
Arguments:   

1. string (sendId)
2. int32 (sendPosition) (-1 end of list)

**TrackRemoveSendNode**  
Type: incoming  
Address: /track/{trackId}/removeSendNode  
Arguments:   

1. string (sendId)
  
**TrackSendRemoved**  
Type: outcoming  
Address: /track/{trackId}/sendRemoved  
Arguments:   

1. string (sendId)

**TrackMoveSendNode** - moves send in list  
Type: incoming  
Address: /track/{trackId}/moveSendNode  
Arguments:   

1. string (sendId)
2. int32 (sendPosition)

**TrackSendMoved**  
Type: outcoming  
Address: /track/{trackId}/sendMoved  
Arguments:   

1. string (sendId)
2. int32 (oldPosition)
3. int32 (newPosition)

**TrackAddPlugin**  
Type: incoming  
Address: /track/{trackId}/addPlugin  
Arguments:   

1. int32 (pluginTypeId)
2. int32 (pluginPosition) (-1 end of list)
  
**TrackPluginAdded**  
Type: outcoming  
Address: /track/{trackId}/pluginAdded  
Arguments:   

1. string (pluginId)
2. int32 (pluginPosition) (-1 end of list)

**TrackRemovePlugin**  
Type: incoming  
Address: /track/{trackId}/removePlugin  
Arguments:
   
1. string (pluginId)
  
**TrackPluginRemoved**  
Type: outcoming  
Address: /track/{trackId}/pluginRemoved  
Arguments:   

1. string (pluginId)

**TrackMovePlugin**  
Type: incoming  
Address: /track/{trackId}/movePlugin  
Arguments:   

1. string (pluginId)
2. int32 (position) (-1 end of list)  
  
**TrackPluginMoved**  
Type: outcoming  
Address: /track/{trackId}/pluginMoved  
Arguments:   

1. string (pluginId)
2. in32 (oldPosition)
3. in32 (newPosition) (-1 end of list)
  
**TrackAddInfoListener**  
Type: incoming  
Address: /track/{trackId}/addInfoListener  
Registers for:

- TrackNameChanged
- TrackColourChanged
  
**TrackRemoveInfoListener**  
Type: incoming  
Address: /track/{trackId}/removeInfoListener
  
**TrackAddControlsListener**  
Type: incoming  
Address: /track/{trackId}/addControlsListener  
Registers for:      

- TrackGainChanged
- TrackPanChanged
- TrackMuteChanged
- TrackSoloChanged
- TrackRecordArmChanged
  
**TrackRemoveControlsListener**  
Type: incoming  
Address: /track/{trackId}/removeControlsListener

**TrackAddPluginsListener**  
Type: incoming  
Address: /track/{trackId}/addPluginsListener  
Registers for:      

- TrackPluginAdded
- TrackPluginRemoved
- TrackPluginMoved
  
**TrackRemovePluginsListener**  
Type: incoming  
Address: /track/{trackId}/removePluginsListener
  
**TrackAddSendsListener**  
Type: incoming  
Address: /track/{trackId}/addSendsListener  
Registers for:      

- TrackSendAdded
- TrackSendRemoved
- TrackSendMoved
  
**TrackRemoveSendsListener**  
Type: incoming  
Address: /track/{trackId}/removeSendsListener


## TrackSendNode messages ##

**SendGetDetail**  
Type: incoming query  
Address: /send/{sendId}/getDetail

**SendDetail**  
Type: outcoming response  
Address: /send/{sendId}/detail  
Arguments:   

1. float32 (gain)
2. float32 (pan)
3. string (name)
4. string (colour) hex value
5. int32 (audioSourcePosition)

**SendSetGain**  
Type: incoming  
Address: /send/{sendId}/setGain  
Arguments:   

1. float32 (gain)

**SendGainChanged**  
Type: outcoming  
Address: /send/{sendId}/gainChanged  
Arguments:   

1. float32 (gain)

**SendSetPan**  
Type: incoming  
Address: /send/{sendId}/setPan  
Arguments:   

1. float32 (pan)
  
**SendPanChanged**  
Type: outcoming  
Address: /send/{sendId}/panChanged  
Arguments:   

1. float32 (pan)
  
**SendSetAudioSourcePosition**  
Type: incoming  
Address: /send/{sendId}/setAudioSourcePosition  
Arguments:   

1. int32 (audioSourcePosition) [0 - before plugins, 1 - 999 after nth plugin, 1000 - after plugins, 1001 - after fader]
  
**SendAudioSourcePositionChanged**  
Type: outcoming  
Address: /send/{sendId}/audioSourcePositionChanged  
Arguments:   

1. int32 (audioSourcePosition) [0 - before plugins, 1 - 999 after nth plugin, 1000 - after plugins, 1001 - after fader]

**SendSetName**  
Type: incoming  
Address: /send/{sendId}/setName  
Arguments:   

1. string (name)
  
**SendNameChanged**  
Type: outcoming  
Address: /send/{sendId}/nameChanged  
Arguments:   

1. string (name)
  
**SendColourChanged**  
Type: outcoming  
Address: /send/{sendId}/colourChanged  
Arguments:   

1. string (colour) hex value

**SendAddInfoListener**  
Type: incoming  
Address: /send/{sendId}/addInfoListener  
Registers for:      

- SendNameChanged
- SendColourChanged
  
**SendRemoveInfoListener**  
Type: incoming  
Address: /send/{sendId}/removeInfoListener
  
**SendAddControlsListener**  
Type: incoming  
Address: /send/{sendId}/addControlsListener  
Registers for:      

- SendGainChanged
- SendPanChanged
  
**SendRemoveControlsListener**  
Type: incoming  
Address: /send/{sendId}/removeControlsListener
  
**SendAddAudioSourcePositionListener**  
Type: incoming  
Address: /send/{sendId}/addAudioSourcePositionListener  
Registers for:      

- SendAudioSourcePositionChanged
  
**SendRemoveAudioSourcePositionListener**  
Type: incoming  
Address: /send/{sendId}/removeAudioSourcePositionListener

 
## Plugin messages ##

**PluginGetDetail**  
Type: incoming query  
Address: /plugin/{pluginId}/getDetail

**PluginDetail**  
Type: outcoming response  
Address: /plugin/{pluginId}/detail  
Arguments:   

1. string (name)
2. string (type)
3. int32 (typeId)
4. int32 (bypased) bool value
5. int32 (active) bool value
  
**PluginGetParameterValues**  
Type: incoming query  
Address: /plugin/{pluginId}/getParameterValues

**PluginParameterValues**  
Type: outcoming response  
Address: /plugin/{pluginId}/parameterValues  
Arguments:   

1. string (values) JSON array of pairs [numericValue, textValue]
  
**PluginGetParameters**  
Type: incoming query  
Address: /plugin/{pluginId}/getParameters
  
**PluginParameters**  
Type: outcoming response  
Address: /plugin/{pluginId}/parameters  
Arguments:   

1. string (parameters) JSON array of tuples [index, ref, value, textValue]

**PluginSetName**  
Type: incoming  
Address: /plugin/{pluginId}/setName  
Arguments:   

1. string (name)

**PluginNameChanged**  
Type: outcoming  
Address: /plugin/{pluginId}/nameChanged  
Arguments:   

1. string (name)
  
**PluginSetActive**  
Type: incoming  
Address: /plugin/{pluginId}/setActive  
Arguments:   

1. int32 (active) bool value

**PluginActiveChanged**  
Type: outcoming  
Address: /plugin/{pluginId}/activeChanged  
Arguments:   

1. int32 (active) bool value

**PluginSetBypass**  
Type: incoming  
Address: /plugin/{pluginId}/setBypass  
Arguments:   

1. int32 (bypassed) bool value

**PluginBypassChanged**  
Type: outcoming  
Address: /plugin/{pluginId}/bypassChanged  
Arguments:   

1. int32 (bypassed) bool value
  
**PluginSetParameterValue**  
Type: incoming  
Address: /plugin/{pluginId}/setParameterValue  
Arguments:   

1. int32 (parameterIndex)
2. float (newValue)

**PluginParameterValueChanged**  
Type: outcoming  
Address: /plugin/{pluginId}/parameterValueChanged  
Arguments:   

1. int32 (parameterIndex)
2. float (newValue)
3. string (newValueText)

**PluginAddListener**  
Type: incoming  
Address: /plugin/{pluginId}/addListener  
Registers for:      

- PluginNameChanged

**PluginRemoveListener**  
Type: incoming  
Address: /plugin/{pluginId}/removeListener

**PluginAddParameterListener**  
Type: incoming  
Address: /plugin/{pluginId}/addParameterListener  
Registers for:      

- PluginParameterValueChanged

**PluginRemoveParameterListener**  
Type: incoming  
Address: /plugin/{pluginId}/removeParameterListener

**PluginAddBypassListener**  
Type: incoming  
Address: /plugin/{pluginId}/addBypassListener  
Registers for:      

- PluginBypassChanged

**PluginRemoveBypassListener**  
Type: incoming  
Address: /plugin/{pluginId}/removeBypassListener

**PluginAddActiveListener**  
Type: incoming  
Address: /plugin/{pluginId}/addActiveListener  
Registers for:      

- PluginActiveChanged

**PluginRemoveActiveListener**  
Type: incoming  
Address: /plugin/{pluginId}/removeActiveListener

## Solo bus messages ##

**SoloBusAddAudioDataListener**  
Type: incoming  
Address: /soloBus/addAudioDataListener  
Arguments:   

1. int32 (minNumSamples) Minimal number of samples in each message  

Registers for:  

- AudioCoreAudioData

**SoloBusRemoveAudioDataListener**  
Type: incoming  
Address: /soloBus/removeAudioDataListener

## PluginManager messages ##

**PluginManagerGetPluginTypes**  
Type: incoming query  
Address: /pluginManager/getPluginTypes

**PluginManagerPluginTypes**  
Type: outcoming response  
Address: /pluginManager/pluginTypes  
Arguments:   

1. string (pluginTypes) JSON array of tuples [pluginTypeId, name, manufacturerName]

**PluginManagerScanPluginTypes**  
Type: incoming  
Address: /pluginManager/scanPluginTypes

**PluginManagerScanPluginTypesIfNeeded**  
Type: incoming  
Address: /pluginManager/scanPluginTypesIfNeeded
  
**PluginManagerPluginTypesScanStarted**  
Type: outcoming  
Address: /pluginManager/pluginManagerPluginTypesScanStarted

**PluginManagerPluginTypesScanned**  
Type: outcoming  
Address: /pluginManager/pluginManagerPluginTypesScanned     
Arguments:   

1. int32 newNumPlugins
  
**PluginManagerPluginTypeAdded** - Called when plugin is imported by user (not during scan)  
Type: outcoming  
Address: /pluginManager/pluginTypeAdded  
Arguments:   

1. int32 pluginTypeId
2. string pluginName
3. string manufacturerName

**PluginManagerAddListener**  
Type: incoming  
Address: /pluginManager/addListener  
Registers for:      

- PluginManagerPluginTypesScanStarted
- PluginManagerPluginTypesScaned

**PluginManagerRemoveListener**  
Type: incoming  
Address: /pluginManager/removeListener

## StateManager messages ##

**StateManagerSaveState**  
Type: incoming query  
Address: /stateManager/saveState
  
**StateManagerLoadState**  
Type: incoming query  
Address: /stateManager/loadState
  
**StateManagerGetState**  
Type: incoming query  
Address: /stateManager/getState

**StateManagerState**  
Type: outcoming response – for save/set/load messages  
Address: /stateManager/state  
Arguments:  

1. string (stateXML)
  
**StateManagerSetState**  
Type: incoming query  
Address: /stateManager/setState  
Arguments:   

1. string (stateXML)
  
**StateManagerProcessResult**  
Type: outcoming response  
Address: /stateManager/processResult  
Arguments:   

1. int32 (processOk) bool value