#ifndef MessageType_h
#define MessageType_h

enum MessageType {
    Heartbeat = 1,
    Matchmaking = 2,
    MatchmackingList = 3,
    MatchrequestInvitation = 4,
    MatchrequestInvite = 5,
    Chat = 6,
    GameStart = 7,
    Move = 8,
    Disconnect = 9,
    ContrincantDisconnect = 10,
    Error = 11,
    GameSync = 12,
    BoardSetAuthority = 13,
    ServerInfo = 14,
    PacketSupport = 15,
    GameRequest = 16,
};

#endif /* MessageType_h */
