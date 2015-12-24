/*
	2004. 3. 22 More details - NPC Modified by the artificial intelligence in real-time value of the event( m_nEventSTATUS ) Transmission.
		. GSV_SET_EVENT_STATUS Add the packet.
		. gsv_NPC_CHAR The member variable packet m_nEventSTATUS More.
		Each NPC Data from the unique event m_nEventSTATUS With reference to the value
		Quest dialog creates a context and give ...

	2004. 3. 24 More details - the event object managed by the World gaekje creation / deletion.
		. gsv_ADD_EVENTOBJ Packet.

	2004. 3. 30 More details - NPC Skill sets in motion a number of skill use packets.
		. gsv_SELF_SKILL, gsv_TARGET_SKILL Packets cNpcSkillMOTION Add Variable
		. Ignore this value if your skills, NPC Skill, the packet size is +1 When the cNpcSkillMOTION[0] Entering the motion number.

	2004. 3. 31 More details - gsv_DAMAGE_OF_SKILL Add a packet
		. gsv_EFFECT_OF_SKILL Applicable state packets skill damage either, but the value is passed
		. gsv_DAMAGE_OF_SKILL Apply the skill damage packet and the packet containing the groin as he ...
		. Skill type 7, 17, sent one result.

	2004. 3. 31 Modifications.
		. gsv_EFFECT_OF_SKILL, gsv_DAMAGE_OF_SKILL Structural changes.
		. Using the object-oriented skills, the skill is applied to an object-oriented ...
		. Skills applied to objects gsv_EFFECT_OF_SKILL, gsv_DAMAGE_OF_SKILL Applied by the packet,
		. The results of the above objects using a skill packet is transmitted after gsv_RESULT_OF_SKILL Is transmitted.

	2004. 4. 1  More details. 
		. tag_ADD_CHAR Add team No. CAI_OBJ :: Is_ALLIED (opponents numbers) to call if true, friendly, false enemy side.

	2004. 4. 2  Modification / additions
		. tag_FIELDITEMBy excluding the remaining time (m_wRemainTIME) item in the structure, change the name to tag_DROPITEM.
		. If the items in the General field as the value for the m_wRemainTIME member variable gsv_ADD_FIELDITEM items time setting.
		. It is produced by tag_DROPITEM of packets that damage the time remaining is set to ITEM_OBJ_LIVE_TIME.
		. Gsv_DAMAGE_OF_SKILL item drop: tag_DROPITEM [0] and enter the value in the. The usage is the same as the regular damage items

	2004. 4. 8  추가 사항
		. Gsv_SKILL_LEARN_REPLY RESULT_SKILL_LEARN_DELETE on the type of additional packet response. ..
		. If the response is RESULT_SKILL_LEARN_DELETE, CUserDATA:: Skill_DELETE (slot, skill) by calling the.

	2004. 4. 13 수정/추가 사항
		. tag_ADD_CHAR 멥버 m_btMoveMODE의 이동 상태값 추가..
		. 승용 아이템 교환시:: cli_ASSEMBLE_RIDE_ITEM, gsv_ASSEMBLE_RIDE_ITEM 패킷 사용.

	2004. 4. 16 수정 사항.
		. wsv_CHAR_LIST패킷에 tagBasicInfo대신 tagCHARINFO로 대체, m_nPartItemIDX 추가.
		. tag_ADD_CHAR 구조체에 소환된 케릭터 구분 기능 추가 ::
		  tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_DEC_LIFE_TIME 일경우 owner_obj_idx = (WORD)pAdjSTATUS[ btIDX++ ];
		  로 설정하면됨.
	    . CLI_APPRAISAL_REQ, GSV_APPRAISAL_REPLY 패킷 추가.

	2004. 4. 20 수정 사항.
		. gsv_MOVE 패킷의 m_bRunMODE => m_btMoveMODE ( tag_ADD_CHAR::m_btMoveMODE와 같은값 )
		. gsv_JOIN_ZONE패킷의 m_dwWorldTIME을 m_dwAccWorldTIME로 변경... 게임 서버의 누적 동작 월드 진행 시간

	2004. 4. 28 수정/추가 사항.
	    . gsv_RESULT_OF_SKILL의 m_nResultCNT를 m_nSKillIDX로 변경.
		. gsv_PARTY_LEVnEXP에 m_btFlagLevelUP 추가.
		. GSV_SKILL_START 패킷 추가 : 서버에서 스킬 케스팅 동작시 전송된다.

	2004. 4. 29 추가 사항.
		. cli_SET_WEIGHT_RATE추가, 클라이언트에서 아이템 소지량 체크해서 현재소지량/최대 소지량 비율이
		. 80%이상 뛰기 불등, 100%이상 이동 불능 판단한다.
		. cli_SET_WEIGHT_RATE받은 패킷은 서버에의해 주변에 gsv_SET_WEIGHT_RATE패킷으로 중계됨.
		. 아바타 조인시 패킷 gsv_AVT_CHAR에 m_btWeightRate에 현재 무계비율 입력되어 있음. 
		. cli_JOIN_ZONE패킷 안에 m_btWeightRate 추가.. 서버에 요청시 m_btWeightRate에 무게 비율을 넣어 전송;

	2004. 5. 1 수정 사항
		. gsv_TARGET_SKILL패킷에 속도보정을 위해 스킬 사용주체 좌표, 타겟 좌표 추가...
		. 추가 멤버: m_wSrvDIST, m_PosTO - gsv_ATTACK 패킷의 속도 보정과 동일하게 적용하여 사용.
		. 타겟 스킬의 경우 앞에서 멈칫하여 gsv_SKILL_START패킷을 기다리던 현상 수정될듯...
		. gsv_MOUSECMD, gsv_ATTACK멤버의 m_PosCUR대신 m_wSrvDIST로 변경.
		. cli_CANTMOVE, gsv_ADJUST_POS, cli_MOUSECMD, gsv_MOUSECMD, cli_JOIN_ZONE, gsv_AVT_CHAR Z값 추가.

	2004. 5. 11 수정 사항.
		. gsv_INVENTORY_DATA패킷에 m_WishITEMS[ MAX_WISH_ITEMS ] 추가, 사용 용도는 개인 상점 오픈시...
		. 개인 상점 거래용 패킷 cli_P_STORE_XXX, gsv_P_STORE_XXXX 추가.
		. cli_SET_WISHITEM 추가 상점등에서 구입 원하는 아이템을 설정할수 있다(인터넷 쇼핑몰처럼..)

	2004. 5. 13. 추가 사항.
		. cli_CHATROOM, wsv_CHATROOM, cli_CHATROOM_MSG, wsv_CHATROOM_MSG 패킷 추가
		. 기능: 월드서버에서 일반 채팅 서버처럼, 게임상에서 동작 - 채팅룸을 제공함.

	2004. 5. 17. 추가 사항.
		. cli_MEMO, wsv_MEMO패킷 추가 케릭터간 쪽지 보내기/받기 기능

	2004. 5. 18. 추가 사항.
		. CLI_MESSENGER, WSV_MESSENGER 패킷 추가..  친구 관리 패킷..
		. 1:1 채팅시는 cli_MESSENGER_CHAT, wsv_MESSENGER_CHAT패킷으로 송/수신 함.

	2004. 5. 31. 추가 사항
		. CLI_CHAR_CHANGE 추가.. 게임 플레이도중 케릭터 선택화면 이동 이동할때 게임서버에 전송..
		. 결과는 월드서버에서 WSV_CHAR_CHANGE 패킷이 전송된. 패킷 구조는 t_PACKETHEADER 사용

	2004. 6. 1.  추가 사항.
		. tag_ADD_CHAR 구조체에 소환된 케릭터에 사용된 소환 스킬 번호 추가 ::
		  기존 tag_ADD_CHAR.m_dwStatusFALG & FLAG_ING_DEC_LIFE_TIME 일경우 owner_obj_idx = (WORD)pAdjSTATUS[ btIDX++ ];
		  에서 owner_obj_idx != 0 이면 nUsedSummonSkillIDX = pAdjSTATUS[ btIDX ++ ]로 얻는다.
		. 소환된 몹의 특성치중 몇몇(MaxHP,공격력,방어력...) 은 소환에 사용된 스킬 번호로 얻을수 있다.

	2004. 6. 8.	 추가 사항.
		. 아이템 수리 요청,결과 패킷 ( 소모성아이템 사용시, NPC에 의한 수리시 ) 추가
		. 아이템 수명 변경및, 수리 결과 패킷은 기존의 gsv_SET_MONEYnINV, gsv_SET_INV_ONLY를 사용해
		. 패킷 타입만 변경되어 전송됨
		. 아이템 수명 변경 패킷 추가 : GSV_SET_ITEM_LIFE => 수명이 0으로 설정될 경우 UpdateAbility()를 호출해줘야 함

	2004. 6. 14. 수정 사항.
		. gsv_TOGGLE, gsv_EQUIP_ITEM, gsv_ASSEMBLE_RIDE_ITEM 패킷에 m_nRunSPEED[0]에 변경시 이동속도 추가...
		. if ( pPacket->m_HEADER.m_nSize == ( sizeof( gsv_XXX ) + sizeof(short) ) ) 이면 m_nRunSPEED[0] 값으로 이동 속도 적용.

	2004. 6. 16. 추가 사항.
		. GSV_P_STORE_MONEYnINV 패킷 타입 추가, 구조는 gsv_SET_MONEYnINV와 동일.
		. gsv_P_STORE_RESULT 결과에 RESULT_P_STORE_TRADE_PART 추가 거래시 일부 품목이 품절되거나 구입 희망 갯수보다
		. 작개 구입시 전송됨..

	2004. 6. 17. 추가 사항.
		. 부활시 패널티 경험치 적용방법 변경으로 gsv_JOIN_ZONE 패킷에 현재 경험치 m_lCurEXP 추가

	2004. 6. 28. 추가 사항.
		. CLI_CHANNEL_LIST_REQ 패킷 추가. srv_LOGIN_REPLY 패킷 수신후 원하는 월드서버의 ID를 전송하면
		. 월드 서버의 채널 서버리스트가 전송됨. 월드서버의 채널을 선택후 cli_SELECT_SERVER패킷에 채널 번호를
		. 추가해서 전송시켜야함.
	2004. 7. 8. 추가 사항.
		. CLI_CRAFT_ITEM_REQ, GSV_CRAFT_ITEM_REPLY 패킷 추가... 클라이언트 요청시에는 각각에 맞는 패킷에
		. 넣어 전송, 서버의 응답 패킷(GSV_CRAFT_ITEM_REPLY)은 공통, m_btRESULT의 값이 성공관련 값일경우에만
		. m_btOutCNT에 유효한 값이 들어있고, m_btRESULT이 실패관련일경우 참조하면 안됨(재련실패는예외-등급감소,소모된 아이템들어있음).
	2004. 7. 10. 추가 사항.
		. wsv_CHAT_ROOM_JOIN 패킷에 자신의 m_wUserID추가, 서버에서 CHAT_REPLY_ROOM_MAKED응답시
		. 방이름 앞에 m_wUserID(자신) 삽입.
		. gsv_CRAFT_ITEM_REPLY패킷중 m_btRESULT값이 CRAFT_UPGRADE_SUCCESS, CRAFT_UPGRADE_FAILED	일경우
		. m_sInvITEM[ m_btOutCNT-1 ].m_iQuantity에 성공도 계산된값이 들어있음(기획서참고)
	2004. 7. 20. 추가 사항.
		. gsv_AVT_CHAR에 ( tag_ADD_CHAR.m_dwStatusFALG & FLAG_ING_STORE_MODE ) 일경우 char szStoreTITLE[] 붙여 전송
		. GSV_SKILL_CANCEL 패킷 추가
	2004. 7. 26. 추가 사항.
		. gsv_PARTY_MEMBER에 파티의 현재 룰 추가.
		. cli_PARTY_ITEM, gsv_PARTY_ITEM 패킷 추가. 파티에서 아이템습득이 순서 분배일경우, 습득자와 아이템 정보들어 있음
	2004. 8. 8 수정사항.
		. MOB의 HP값을 short에서 int로 변경.
	2004. 8. 10 추가 사항.
		. tagCHARINFO 구조체에 m_dwRemainSEC멤버변수 추가, 값이 0이 아닐경우 삭제될때까지 남은 시간(초단위).
		. cli_DELETE_CHAR패킷에 m_bDelete 멤버변수 추가, 서버에 요청시 true면 삭제대기등록, false면 삭제대기 해제,
		. 응답 패킷으로 wsv_DELETE_CHAR 전송됨.
	2004. 8. 23 추가 사항.
		. gsv_SETEXP 패킷에 현재 스테미너 값 추가
	2004. 9. 16 추가 사항.
		. 파티 멥버 정보 tag_PARTY_MEMBER에 m_nSTAMINA 추가.
		. 추가로 파티 원이 스테미너가 < 3000 이하일경우 ... gsv_CHANGE_OBJIDX 패킷이 추가 전송됨
	2004. 10. 4 추가 사항.
		. gsv_SET_HPnMP패킷 추가 : 서버의 현재 hp, mp가 전송됨
	2004. 10. 5 추가 사항.
		. gsv_CHECK_NPC_EVENT패킷 추가, 기존 NPC죽였을때 클라이언트에서 NPC이벤트를 체크후 서버에 검증 요청하던것을
		. 서버가 클라이언트에게 검증요청 => 클라이언트 검증후 검증완료되면 => 서버에 검증요청 순서로 바꿈.
		. 마지막에 몹을 죽였던 사용자에게 들어가던 퀘스트아이템을 선빵때린 유저에게 들어가도록...
	2004. 10. 15 수정 / 추가 사항.
		. tag_ADD_CHAR 구조체의 char m_cTeamNO가 int m_iTeamNO로 변경
		. gsv_JOIN_ZONE 구조체에 자신의 팀번호 추가 int m_iTeamNO;
	2004. 10. 30 추가 사항.
		. 클랜 패킷 추가 CLI_CLAN_COMMAND, WSV_CLAN_COMMAND
		. 클랜관련 패킷은 모두 월드 서버에 전송함.
	2004. 11. 16 추가 / 수정.
		. CLI_ALLIED_CHAT, CLI_ALLIED_SHOUT, GSV_ALLIED_CHAT, GSV_ALLIED_SHOUT 추가
		. PvP 존에서 같은 팀번호에게만 채팅을 전송한다.
		. gsv_AVT_CHAR구조체의 m_dwSubFLAG추가 DataType.h의 FLAG_SUB_xxx 플레그 설정됨
		. tag_MY_CLAN구조체안의 m_btClanPOS(클랜내 직위)를 tag_CLAN_ID 구조체로 이동.
		. 서버에 보내는 클랜 명령에 GCMD_MEMBER_JOBnLEV추가 :: 자신의 직업/레벨을 다른 멤버에게 통보( 로그인후/레벨업후/전직후에 전송 )
	2004. 11. 23 수정.
		. tag_CLAN_INFO 삭제, tag_MY_CLAN 멥머 추가
	2004. 12. 1 추가
		. 10초 대기해야할경우 GSV_LOGOUT_REPLY 전송
	2004. 12. 16 추가
		. 제조/제련시 주위에 통보 패킷( cli_ITEM_RESULT_REPORT ) 추가, cli_CREATE_ITEM_EXP_REQ 패킷은 삭제
	2004. 12. 21 추가 
		. gsv_EFFECT_OF_SKILL에 스킬 시전자의 지력 m_nINT값 추가.
	2005. 01. 05 추가
		. CLI_SELECT_SERVER요청시 응답 결과 RESULT_SELECT_SERVER_NEED_CHARGE 추가 (무료사용자가 본섭에 접속시도할때)
		. 과금 관련 패킷 추가. gsv_BILLING_MESSAGE
	2005. 01. 11 수정
		. gsv_CHANGE_SKIN수정, gsv_SET_ABILITY패킷은 자신한테만 전달되고 주위에 통보 필요한 경우(머리털,얼굴,성별) gsv_CHANGE_SKIN로 전송됨
		. tagCHARINFO 서버에서 케릭터 리스트에 플레티넘케릭터 여부 필드 추가
		. wsv_CREATE_CHAR케릭터 생성 패킷에.... 플레티넘 케릭으로 생성된건지 여부...
		. gsv_BANK_LIST_REPLY패킷에 BANK_REPLY_PLATINUM 추가...플레티넘 창고리스트가 별도로 전송됨(아이템이 존제할경우)
	2005. 03. 23 추가
		. 클랜 마크 요청 패킷 cli_CLANMARK_REQ, wsv_CLANMARK_REPLY 추가
	2005. 06. 07 수정/추가
		. GSV_NPC_CHAR 패킷시 m_nCharIdx가 -(음수) 이면 숨김 상태이다.
		. gsv_SET_NPC_SHOW NPC의 숨김/보임 상태 설정.
	2005. 06. 16 추가
		. 기존 gsv_BILLING_MESSAGE에서 확장된 형태의 gsv_BILLING_MESSAGE_EXT 패킷 추가
		. defined FLAG_PAY_XXX 형태가 PAY_FLAG_국가_XXX 로 이름 변경
	2005. 06. 20 추가
		. 여신 소환 관련 패킷 GSV_GODDNESS_MODE 추가.
	2005. 06. 22 수정
		. cli_TELEPORT_REQ 패킷에 스피드핵을 방지하기 위해 서버와 비교할 좌표 클라이언트의 현재 좌표(m_PosCUR) 추가
	2005. 06. 23 추가
		. 파티 / 1:1 거래시 과금없을시 응답 추가
		. 소환수 명령 패킷 추가.( CLI_SUMMON_CMD, GSV_SUMMON_CMD )
	2005. 08. 22 추가
		. CLI_CART_RIDE / GSV_CART_RIDE 2인승카트 요청/응답 패킷 추가.
*/		  
#ifndef __NET_PROTOTYPE_H
#define __NET_PROTOTYPE_H
#ifdef	__SERVER
	#include "DataTYPE.h"
	#include "CUserDATA.h"
	#include "PacketHEADER.h"
	#include "classHASH.h"
#else
	#include "../common/DataTYPE.h"
	#include "../common/CUserDATA.h"
	#include "../util/PacketHEADER.h"
	#include "../util/classHASH.h"
#endif



#define	__APPLY_04_10_15_TEAMNO			// 04.10.15 Wanna team number change date applies packet ?
#define	__INC_PLATINUM				// Whether to apply changes monetization.



#pragma warning( disable:4200 )
#pragma warning( disable:4201 )
#pragma pack (push, 1)

//-------------------------------------------------------------------------------------------------

#define	CLI_ALIVE					0x0700
#define	SRV_ERROR					0x0700

#define	SRV_ANNOUNCE_TEXT			0x0701
#define	GSV_ANNOUNCE_CHAT			0x0702

#define	CLI_ACCEPT_REQ				0x0703

#define	CLI_CHANNEL_LIST_REQ		0x0704
#define	LSV_CHANNEL_LIST_REPLY		0x0704

#define	CLI_CHECK_AUTH				0x0705
#define	SRV_CHECK_AUTH				0x0705

// 0x0706

#define	CLI_LOGOUT_REQ				0x0707
#define GSV_LOGOUT_REPLY			0x0707

#define CLI_LOGIN_REQ				0x0708
#define LSV_LOGIN_REPLY				0x0708
#define	GSV_LOGIN_REPLY				0x0709

#define	CLI_SELECT_SERVER			0x070a
#define	LSV_SELECT_SERVER			0x070a

/*
#define	CLI_JOIN_WORLD				0x070b
#define WSV_JOIN_WORLD				0x070c		// Verify whether the connection from the server ... World
*/

#define	CLI_JOIN_SERVER_REQ			0x070b		// World / zone server to request a connection.
#define	SRV_JOIN_SERVER_REPLY		0x070c


#define	GSV_GM_COMMAND				0x070d		// GM Command.

#define	GSV_SET_GLOBAL_VAR			0x070e		// Server Variables.
#define	GSV_SET_GLOBAL_FLAG			0x070f		// Server flags value...

#define	WSV_MOVE_SERVER				0x0711		// John server moving, guys....

#define	CLI_CHAR_LIST				0x0712
#define	WSV_CHAR_LIST				0x0712

#define	CLI_CREATE_CHAR				0x0713
#define	WSV_CREATE_CHAR				0x0713

#define	CLI_DELETE_CHAR				0x0714
#define WSV_DELETE_CHAR				0x0714

#define	CLI_SELECT_CHAR				0x0715
#define	GSV_SELECT_CHAR				0x0715


#define	GSV_INVENTORY_DATA			0x0716
#define	GSV_SET_MONEYnINV			0x0717
#define	GSV_SET_INV_ONLY			0x0718


#define	GSV_SERVER_DATA				0x0719

#define	GSV_RELAY_REQ				0x071a	
#define	CLI_RELAY_REPLY				0x071a

#define	GSV_QUEST_DATA				0x071b

#define	CLI_CHAR_CHANGE				0x071c		// Request :: character selection window, go to the game server haeyaham packet transmission.
#define	WSV_CHAR_CHANGE				0x071c		// CLI_CHAR_LIST the server receives a request packet sent from the server :: World doeeojim

#define GSV_SET_MONEY_ONLY			0x071d		// Server's current money... 

#define	GSV_REWARD_MONEY			0x071e		// Structure of compensation money was ... gsv_SET_MONEY_ONLY
#define	GSV_REWARD_ITEM				0x071f		// The item was compensated
#define	GSV_REWARD_ADD_ABILITY		0x0720		// More stats
#define	GSV_REWARD_SET_ABILITY		0x0721		// Base Set

#define	CLI_LOGOUT_CANCEL			0x0722

#define	GSV_QUEST_ONLY				0x0723
#define	GSV_WISH_LIST				0x0724


#define	CLI_QUEST_REQ				0x0730
#define	GSV_QUEST_REPLY				0x0730

#define	GSV_CHECK_NPC_EVENT			0x0731

#define	GSV_CHEAT_CODE				0x0751


#define	CLI_JOIN_ZONE				0x0753
#define	GSV_JOIN_ZONE				0x0753

#define GSV_INIT_DATA				0x0754

#define	CLI_REVIVE_REQ				0x0755
#define	GSV_REVIVE_REPLY			0x0755

#define	CLI_SET_REVIVE_POS			0x0756

#define	CLI_SET_VAR_REQ				0x0757
#define	GSV_SET_VAR_REPLY			0x0757

#define CLI_CHAR_INFO_REQ			0x0761
#define	GSV_CHAR_INFO_REPLY			0x0761

#define	CLI_SET_WEIGHT_RATE			0x0762
#define	GSV_SET_WEIGHT_RATE			0x0762

#define	GSV_ADJUST_POS				0x0770


#define	CLI_CANTMOVE				0x0771
#define	GSV_CANTMOVE				0x0771

// #define	CLI_ATTACK_START			0x0772
// #define	GSV_ATTACK_START			0x0772
// #define	GSV_ATTACK_STOP				0x0773

#define	GSV_CHANGE_NPC				0x0774

#define	CLI_SUMMON_CMD				0x0775
#define	GSV_SUMMON_CMD				0x0775

// 0x77

#define	CLI_SET_MOTION				0x0781
#define	GSV_SET_MOTION				0x0781

#define	CLI_TOGGLE					0x0782
#define	GSV_TOGGLE					0x0782

#define	CLI_CHAT					0x0783
#define	GSV_CHAT					0x0783

#define	CLI_WHISPER					0x0784
#define	GSV_WHISPER					0x0784

#define	CLI_SHOUT					0x0785
#define	GSV_SHOUT					0x0785

#define	CLI_PARTY_CHAT				0x0786
#define GSV_PARTY_CHAT				0x0786

#define	CLI_CLAN_CHAT				0x0787
#define	WSV_CLAN_CHAT				0x0787

#define	CLI_ALLIED_CHAT				0x0788
#define GSV_ALLIED_CHAT				0x0788

#define	CLI_ALLIED_SHOUT			0x0789
#define	GSV_ALLIED_SHOUT			0x0789

#define	GSV_SET_EVENT_STATUS		0x0790

#define	GSV_NPC_CHAR				0x0791
#define	GSV_MOB_CHAR				0x0792
#define	GSV_AVT_CHAR				0x0793

#define GSV_SUB_OBJECT			    0x0794

#define CLI_SETPOS				    0x0795

#define	CLI_STOP					0x0796
#define	GSV_STOP					0x0796

#define GSV_MOVE					0x0797

#define	CLI_ATTACK					0x0798
#define GSV_ATTACK					0x0798

#define	CLI_DAMAGE					0x0799			//combat damage
#define GSV_DAMAGE					0x0799

#define	CLI_MOUSECMD				0x079a
#define	GSV_MOUSECMD				0x079a

#define	GSV_SETEXP					0x079b

#define GSV_LEVELUP					0x079e

#define	CLI_HP_REQ					0x079f
#define	GSV_HP_REPLY				0x079f			//Set enemies health

#define	GSV_SET_HPnMP				0x07a0

#define	CLI_STORE_TRADE_REQ			0x07a1
#define	GSV_STORE_TRADE_REPLY		0x07a1

//#define	CLI_CREATE_ITEM_EXP_REQ		0x07a2

#define	CLI_USE_ITEM				0x07a3
#define GSV_USE_ITEM				0x07a3

#define	CLI_DROP_ITEM				0x07a4

#define	CLI_EQUIP_ITEM				0x07a5
#define	GSV_EQUIP_ITEM				0x07a5

#define	GSV_ADD_FIELDITEM			0x07a6
#define	GSV_SUB_FIELDITEM			0x07a6

#define	CLI_GET_FIELDITEM_REQ		0x07a7
#define GSV_GET_FIELDITEM_REPLY		0x07a7

#define CLI_TELEPORT_REQ			0x07a8
#define GSV_TELEPORT_REPLY			0x07a8

#define	CLI_USE_BPOINT_REQ			0x07a9
#define GSV_USE_BPOINT_REPLY		0x07a9

#define	CLI_SET_HOTICON				0x07aa
#define	GSV_SET_HOTICON				0x07aa

#define	CLI_SET_BULLET				0x07ab
#define	GSV_SET_BULLET				0x07ab

//#define	CLI_CHANGE_SKIN				0x07ac
#define	GSV_CHANGE_SKIN				0x07ac

#define	CLI_BANK_LIST_REQ			0x07ad
#define	GSV_BANK_LIST_REPLY			0x07ad

#define	CLI_MOVE_ITEM				0x07ae
#define	GSV_MOVE_ITEM				0x07ae

#define	CLI_CREATE_ITEM_REQ			0x07af
#define	GSV_CREATE_ITEM_REPLY		0x07af


#define GSV_SKILL_LEARN_REPLY		0x07b0

#define	CLI_SKILL_LEVELUP_REQ		0x07b1
#define GSV_SKILL_LEVELUP_REPLY		0x07b1


#define	CLI_SELF_SKILL				0x07b2
#define	GSV_SELF_SKILL				0x07b2

#define	CLI_TARGET_SKILL			0x07b3
#define	GSV_TARGET_SKILL			0x07b3

#define	CLI_POSITION_SKILL			0x07b4
#define	GSV_POSITION_SKILL			0x07b4

#define	GSV_EFFECT_OF_SKILL			0x07b5
#define	GSV_DAMAGE_OF_SKILL			0x07b6

#define	GSV_CLEAR_STATUS			0x07b7
#define	GSV_SPEED_CHANGED			0x07b8

#define	GSV_RESULT_OF_SKILL			0x07b9

#define	CLI_APPRAISAL_REQ			0x07ba		// Referred emotion items ~ ~ ~
#define	GSV_APPRAISAL_REPLY			0x07ba

#define	GSV_SKILL_START				0x07bb		// Start casting skills...

#define	CLI_CRAFT_ITEM_REQ			0x07bc
#define	GSV_CRAFT_ITEM_REPLY		0x07bc

#define	GSV_SKILL_CANCEL			0x07bd		// Skills began Cancel.
//	0x07be

#define	CLI_SET_WISHITEM			0x07bf

#define	CLI_TRADE_P2P				0x07c0		// Interpersonal transactions...
#define GSV_TRADE_P2P				0x07c0

#define	CLI_TRADE_P2P_ITEM			0x07c1		// Trading Items Add / Remove.
#define	GSV_TRADE_P2P_ITEM			0x07c1

#define CLI_P_STORE_OPEN			0x07c2		// Individual store opening request
#define GSV_P_STORE_OPENED			0x07c2

#define CLI_P_STORE_CLOSE			0x07c3		// Individual shops closed on request
#define	GSV_P_STORE_CLOSED			0x07c3

#define CLI_P_STORE_LIST_REQ		0x07c4		// LIST request personal store
#define GSV_P_STORE_LIST_REPLY		0x07c4

#define CLI_P_STORE_BUY_REQ			0x07c5		// Individual shop goods purchase request
#define	CLI_P_STORE_SELL_REQ		0x07c6		// Individual shops selling goods on request

#define	GSV_P_STORE_RESULT			0x07c6
#define	GSV_P_STORE_MONEYnINV		0x07c7

#define	CLI_ASSEMBLE_RIDE_ITEM		0x07ca
#define	GSV_ASSEMBLE_RIDE_ITEM		0x07ca

#define	CLI_USE_ITEM_TO_REPAIR		0x07cb		// Consumable items repair repair request item
#define	GSV_USED_ITEM_TO_REPAIR		0x07cb		// gsv_SET_INV_ONLY

#define	CLI_REPAIR_FROM_NPC			0x07cd		// NPC Repair request items through
#define	GSV_REPAIRED_FROM_NPC		0x07cd		// gsv_SET_MONEYnINV

#define	GSV_SET_ITEM_LIFE			0x07ce		// The life of the modified item

#define	CLI_PARTY_REQ				0x07d0
#define	GSV_PARTY_REQ				0x07d0

#define	CLI_PARTY_REPLY				0x07d1
#define	GSV_PARTY_REPLY				0x07d1

#define	GSV_PARTY_MEMBER			0x07d2		// Party Add / Remove.
//#define	GSV_PARTY_INFO			0x07d3		// Party Information
#define	GSV_PARTY_ITEM				0x07d3		// Information acquired during the distribution item order

#define	GSV_PARTY_LEVnEXP			0x07d4

#define	GSV_CHANGE_OBJIDX			0x07d5


#define	GSV_ADD_EVENTOBJ			0x07d6
// #define	GSV_SUB_EVENTOBJ			0x07d6

#define	CLI_PARTY_RULE				0x07d7
#define	GSV_PARTY_RULE				0x07d7

#define	CLI_ITEM_RESULT_REPORT		0x07d8
#define	GSV_ITEM_RESULT_REPORT		0x07d8

#define	CLI_MALL_ITEM_REQ			0x07d9
#define	GSV_MALL_ITEM_REPLY			0x07d9


#define	CLI_MOVE_ZULY				0x07da
#define	GSV_MOVE_ZULY				0x07da

#define	GSV_SET_NPC_SHOW			0x07db

#define	GSV_GODDNESS_MODE			0x07dc


#define	CLI_CART_RIDE				0x07dd
#define	GSV_CART_RIDE				0x07dd


#define	GSV_BILLING_MESSAGE_EXT		0x07de
#define	GSV_BILLING_MESSAGE			0x07df


#define CLI_CLAN_COMMAND			0x07e0
#define	WSV_CLAN_COMMAND			0x07e0

#define	CLI_MESSENGER				0x07e1
#define	WSV_MESSENGER				0x07e1

#define	CLI_MESSENGER_CHAT			0x07e2
#define	WSV_MESSENGER_CHAT			0x07e2
									   
#define	CLI_CHATROOM				0x07e3
#define	WSV_CHATROOM				0x07e3
									   
#define	CLI_CHATROOM_MSG			0x07e4
#define	WSV_CHATROOM_MSG			0x07e4

#define	CLI_MEMO					0x07e5
#define	WSV_MEMO					0x07e5

#define	CLI_CLANMARK_SET			0x07e6

#define	CLI_CLANMARK_REQ			0x07e7
#define	WSV_CLANMARK_REPLY			0x07e7

#define CLI_CLANMARK_REG_TIME		0x07e8
#define	WSV_CLANMARK_REG_TIME		0x07e8

#define	GSV_PATSTATE_CHANGE			0x07e9

#define	GSV_CHARSTATE_CHANGE		0x07ea


#define CLI_SCREEN_SHOT_TIME  0x07eb						//Requests the server time when taking screenshots .. (more heojaeyoung 2005.10.18)
#define GSV_SCREEN_SHOT_TIME 0x07eb							//Transfer the required server time when taking screenshots ... (more heojaeyoung 2005.10.18)


#define GSV_CHAR_HPMP_INFO			0x07ec					//1 Per second from the server to the client MP/HP Send...  
#define GSV_CHAR_STAT_INFO			0x07ed					//PY: New packet added to sens ALL stats from the server


#define GSV_MILEAGE_INV_DATA		0x801					//Mileage inventory data..
#define GSV_DESTROY_MILEAGE_ITEM	0x802	    			//Destroyed items mileage.
#define GSV_NOTIFY_MIEAGE_ITEM		0x803			 		//Mileage changed item information
#define GSV_SET_MILEAGE_INV_ONLY	0x0804					//Setting mileage inventory..


#define CLI_USE_MILEAGE_ITEM		0x805
#define GSV_USE_MILEAGE_ITEM		0x805					//Mileage used items

#define CLI_EQUIP_MILEAGE_ITEM		0x806          			//Mileage wear items
#define GSV_EQUIP_MILEAGE_ITEM		0x806          			//Mileage wear items

#define CLI_EQUIP_RIDE_MILEAGE_ITEM 0x807   				//Mileage cart items worn
#define GSV_EQUIP_RIDE_MILEAGE_ITEM 0x807  					//Mileage cart items worn

//-------------------------------------
// 2006.03.29/Born naturally / more - Arcfour Encryption
#define CLI_DUMMY_1					0x808	// Encryption Related dummy packet 1 - The size of a packet of any size within a certain range - No server response.
//-------------------------------------


//-------------------------------------------------------------------------------------------------
//
//  Keulraenjeon related - 2006.07.24 Kim, YH-related message headers, add keulraenjeon
//
//-------------------------------------------------------------------------------------------------
#define GSV_CLANWAR_INTO_OK			0x0810		// Check keulraenjeon participate [Server => Cloud]
#define CLI_CLANWAR_IN_OK			0x0811		// Participation keulraenjeon OK message [Climb => Server]
#define GSV_CLANWAR_TIME			0x0812		// Keulraenjeon time message [Server => Climb;
#define GSV_CLANWAR_ERR				0x0813		// Error-handling messages keulraenjeon [Server => Cloud]

// (20070117:South byeongcheol) : Keulraenjeon warning message
enum CWAR_WARNING_MSGCODE
{
	CWAR_OPPONENT_MASTER_ALREADY_GIVEUP = 1,		// Zhang clan already abandoned when the relative
	CWAR_QUEST_LIST_FULL,												// When applying for a list of Clan Quest FULL (one in the quest to use keulraenjeon)
	CWAR_CLAN_MASTER_LEVEL_RESTRICTED,					// Clan master level limits
	CWAR_TOO_MANY_CWAR_PROGRESS,								// Keulraenjeon limited number of participants
	CWAR_TIME_OUT,															// Participation keulraenjeon accept timeout.
	CWAR_OPPONENT_MASTER_GIVEUP,								// 974 : [Keulraenjeon] Clan master is waived by the other party to win in November keulraenjeon.	
	CWAR_MASTER_GIVEUP,													// 975 : [Keulraenjeon] keulraenjeon to defeat the clan master has waived September
	CWAR_WARNING_MSGCODE_MAX
};

#define GSV_CLANWAR_PROGRESS		0x0814		// Keulraenjeon progress message type [server => Cloud]

#define GSV_WARNING_MSGCODE			0X0815		//						[Servers -> Climb]

//          2006.11.9 Kim, YH keulraenjeon Adding secondary content
#define GSV_CLANWAR_BOSS_HP         0x0816   // HP boss keulraenjeon transfer    [Server => Cloud] sent periodically



//	2006.09.18 Kim, YH user's access important message header
//	__MESSAGE_LV_1 Defined by the first-class messaging.
#define GSV_USER_MSG_1LV		0x0820	// First-class messaging ... header.


#ifdef __EXPERIENCE_ITEM
#define CLI_MILEAGE_ITEM_TIME	0x0821	// [Climb -> Servers]
#define GSV_MILEAGE_ITEM_TIME	0x0822	// [Servers -> Climb]
#endif

// 2007.02.13/Born naturally / more - the client and the server time synchronization
#define CLI_SERVER_TIME		0x0825
#define GSV_SERVER_TIME		0x0826

#define CLI_LOGIN_REQ2		0x0827
#define LSV_LOGIN_REPLY2	0x0828

//=================================================================================================
//-------------------------------------------------------------------------------------------------
//
//	Set keulraenjeon related packet structure - Kim, YH 2006.07.24 More
//
//-------------------------------------------------------------------------------------------------
// Keulraenjeon request entry
struct gsv_cli_CLAN_WAR_OK : public t_PACKETHEADER
{
	DWORD	m_Sub_Type;		// Additional option (0: Error 1: Clan Master check progress, 2: Clan participation)
							//				 .... The error code HIWORD Values ??determined.
	DWORD	m_Clen_N;		// Clan Code
	WORD	m_Zone_N;		// Arrangement for zone number (1 to charae ..)
	WORD	m_Team_N;		// Content team number. (A team = 1000, B team = 2000)
};

// Related to the remaining time keulraenjeon
struct gsv_cli_CLAN_WAR_TIME : public t_PACKETHEADER
{
	DWORD	m_Play_Type;	// 1: Game progress time (20 minutes), 2: Game End Latency (2 min.)
	DWORD	m_Play_Count;	// The remaining counts - 10 units (ie 1 120 is 10 seconds, then 20 min.)
};

// Keulraenjeon related error handling
struct gsv_cli_CLAN_WAR_ERR : public t_PACKETHEADER
{
	WORD	m_CW_Err_w;		// Keulraenjeon error number
};

//	Message packets proceeds keulraenjeon
struct gsv_cli_CLAN_WAR_PROGRESS : public t_PACKETHEADER
{
	//	1 = Keulraenjeon start, 2 = A team wins, 3 = B team wins, 4 = timeout set-off, ...
	WORD	m_CW_Progress;		// Keulraenjeon proceeding No.
};

//          Keulraenjeon fitness mob boss sent packets

struct gsv_cli_CLAN_WAR_BOSS_HP : public t_PACKETHEADER 
{
	short		m_A_nCharIdx;			// A Team boss mobs Index
	short		m_B_nCharIdx;			// B Team boss mobs Index
	int         m_A_Boss_Max_HP;        // A The maximum strength of the team boseumop
	int         m_A_Boss_HP;            // A The maximum strength of the team boseumop
	int         m_B_Boss_Max_HP;        // B The maximum strength of the team boseumop
	int         m_B_Boss_HP;            // B The maximum strength of the team boseumop
	//          m_B_Boss_HP = 0 M_B_Boss_Max_HP = 0 or if it is shown that the body be dead.
};


//-------------------------------------------------------------------------------------------------
//
//	...
//
//-------------------------------------------------------------------------------------------------
// Related encryption dummy packet 1 to the size of the packet is in an arbitrary size within a predetermined range
struct cli_DUMMY_1 : public t_PACKETHEADER 
{
	BYTE m_Dummy[128];
} ;


struct cli_CHECK_AUTH : public t_PACKETHEADER 
{
	/*
		For each module-specific data structures ... Philippines GG_AUTH_DATA
	*/
} ;
#define	AUTH_MODULE_nPROTECT	0xf1
struct srv_CHECK_AUTH : public t_PACKETHEADER 
{
	BYTE	m_btModuleTYPE;
	/*
		Each module-specific data..
		In the case of the Philippines GG_AUTH_DATA structure
	*/
} ;

struct srv_ERROR : public t_PACKETHEADER 
{
	WORD	m_wErrorCODE;
	// szErrorMSG[]
} ;


struct lsv_ERROR : public srv_ERROR 
{
} ;
struct gsv_ERROR : public srv_ERROR 
{
} ;


struct gsv_GM_COMMAND : public t_PACKETHEADER 
{
	BYTE	m_btCMD;
	WORD	m_wBlockTIME;
} ;
#define	GM_CMD_SHUT		0x00		//	Chat prohibited.
#define	GM_CMD_LOGOUT	0x01		//	Nagara hang-up ...


struct cli_ACCEPT_REQ : public t_PACKETHEADER 
{
} ;

struct cli_LOGIN_REQ : public t_PACKETHEADER 
{
//	DWORD		m_dwBuildNo;							// Client build number.
	union 
	{
		BYTE	m_MD5Password[ 32 ];
		DWORD	m_dwMD5[ 8 ];
	} ;
    // szAccount[];
} ;
struct srv_LOGIN_REPLY : public t_PACKETHEADER 
{
    BYTE    m_btResult;
	WORD	m_wRight;
	WORD	m_wPayType;
    /*  m_cResult == RESULT_LOGIN_REPLY_OK One case
        struct {
            // szServerNAME[]
			DWORD dwServerID;
        } ;
    */
} ;

#define RESULT_LOGIN_REPLY_OK                   0x00
#define	RESULT_LOGIN_REPLY_KOREA_OK				RESULT_LOGIN_REPLY_OK

#define RESULT_LOGIN_REPLY_FAILED               0x01    // error
#define RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT    0x02    // Account no.
#define RESULT_LOGIN_REPLY_INVALID_PASSWORD     0x03    // Off-duty error
#define RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN     0x04    // Already being logged
#define RESULT_LOGIN_REPLY_REFUSED_ACCOUNT      0x05    // The account that was rejected by the server.
#define RESULT_LOGIN_REPLY_NEED_CHARGE          0x06    // Charging is required
#define	RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT	0x07	// There is no access rights.
#define	RESULT_LOGIN_REPLY_TOO_MANY_USER		0x08	// Personnel access limited
#define	RESULT_LOGIN_REPLY_NO_REAL_NAME			0x09
#define	RESULT_LOGIN_REPLY_INVALID_VERSION		0x0a	// It must be a patch.
#define	RESULT_LOGIN_REPLY_OUT_OF_IP			0x0b	// Internet cafe Accessible exceed ip ...

#define	RESULT_LOGIN_REPLY_TAIWAN_OK			0x10	// m_wPayType Taiwan value accounting server system
#define	RESULT_LOGIN_REPLY_TAIWAN_FAILED		0x11	// Taiwan applied to the accounting server system

#define	RESULT_LOGIN_REPLY_JAPAN_OK				0x18	// m_wPayType The value accounting server system in Japan


/*
If the value of Japanese accounting server RESULT_LOGIN_REPLY_JAPAN_OK m_wPayType
PAY_FLAG_JP_???? And to TRUE, billing and operations will
Yes:: )
	if ( m_wPayType & PAY_FLAG_JP_EXTRA_CHAR ) {
		// Five characters, like South Korea Platinum operable charges.
	}
*/
/*
Taiwan RESULT_LOGIN_REPLY_TWAIN_OK days if the value of the charging server m_wPayType
0  :	// prohibition
1	:	// Time - every time the unit 1 to 6 point reduction, if not have to fill an hour, calculated to be one hour.
2	:	// Month Subscription - every 350 points reduced to 30 days.
3	:	// Subscription period - ao 20 point reduction in 12-hour increments.
4	:	// Subscription 3 months - every 90 days as a 888 point reduction.
5	:	// Hong Kong May Subscription
6	:	// Hong Kong three-month flat-rate
7	:	// Hong Kong time
8	:	// Hong Kong time flat-rate
9	:	// Min 0.25 points every 1 minute increments of quantitative-cuts.
10	:	// Time unit of dosage-every 2 hours 12-point cuts.
11	:	// 10-150-point cut every 10 days.
12	:	// Free trial
101:	// Block 3,
102:	// Block 7 days
103:	// Block 10

Taiwan billing server If the value of RESULT_LOGIN_REPLY_TWAIN_FAILED, m_wPayType
2	:	// Duplicate login (login result) is already logged in to your account
3	:	// 패스워드 에러 (login result)	계정 또는 패스워드 에러 또는 포인트 충전하지 않은 상태,  Joypark 회원 중심 참고.
4	:	// ObjnetObiect 구축 실패, Login Server등록 불가능 (Connection State Result)
5	:	// Login Server연결 중단 (Connection State Result)
6	:	// Login Server연결 실패 (Connection State Result)
7	:	// 없는 계정 (login result)	계정 또는 패스워드 에러 또는 포인트를 충전하지 않은 상태,  Joypark 회원 중심 참고. 
8	:	// 포인트 부족 (login result)	포인트 부족
9	:	// 시간 부족 (login result)	무료 체험기간 만기
10	:	// 없는 과금제도 (login result)	과금방식 문제
11	:	// 블록 (login result)	블록 중
12	:	// 블록３일 (login result)	블록 중
13	:	// 블록７일 (login result)	블록 중
14	:	// 블록 10 일 (login result)	블록 중
15	:	// 로그 아웃 상태 (logout result)
16	:	// 불확실한 에러 (데이터 베이스 에러 login, relogin, logout result)	불확실한 에러
17	:	// Ip중복 (홈 유저 대상 login, relogin, result) 
18	:	// 게임방 유효한 게임방 ip리스트 (사용 : 게임방 login, relogin, result)	
20	:	// 없는 계정 (relogin result)	계정 또는 패스워드 에러 도는 포인트를 충전하지 않은 상태, Joypark 회원 중심 참고.
21	:	// 중복 로그인 (relogin result)	이미 온라인 상황인 계정 
23	:	// 포인트 부족 (relogin result)	포인트 부족
24	:	// 시간 부족 (relogin result)	무료 체험기간 만기
25	:	// 부정확한 과금방식 (relogin result)	과금방식 문제 
26	:	// 블록 (relogin result)	블록 중
27	:	// 블록 ３일 (relogin result)	블록 중
28	:	// 블록 ７일 (relogin result)	블록 중
29	:	// 블록 10 일 (relogin result)	블록 중
30	:	// 같은 ip  n초내 중복 로그인 (login result)	마우스를 잠깐 쉬게 하고, 몇 초 후 다시 접속.
31	:	// Game Server등록 성공 (Connection State result)
32	:	// Game Server등록 ID중복(Connection State result)
33	:	// Game Server미 등록(login, relogin result)	서버 접속 불가능, 잠시후 다시 연결 시도.
34	:	// 유저 매일 접속 가능한 시간 초과.(login, relogin result)	접속시간 초과. 
35	:	// 무료 체험제 미 오픈 (login result)	이 서버는 무료 체험 계정 제공하지 않음.
101:	// Library버전과 Login Server 와 맞지 않음. (Connection State result)
*/


struct cli_CHANNEL_LIST_REQ : public t_PACKETHEADER 
{
	DWORD	m_dwServerID;
} ;

struct tagCHANNEL_SRV 
{
	BYTE	m_btChannelNO;		// Show channel slot number
	BYTE	m_btLowAGE;			// Connectable minimum age 0, no minimum age limit
	BYTE	m_btHighAGE;		// Connectable maximum age 0, no maximum age limit
	short	m_nUserPERCENT;		// The amount that is currently connected users ...
	// char m_szChannenNAME[]	// m_szChannelsNAME == "" If CHANNEL- should give the channel number
} ;
struct lsv_CHANNEL_LIST_REPLY : public t_PACKETHEADER 
{
	DWORD			m_dwServerID;
	BYTE			m_btChannelCNT;
	/*
	{
		tagCHANNEL_SRV X m_btChannelCNT;
	}
	*/
} ;

struct cli_SELECT_SERVER : public t_PACKETHEADER 
{
	DWORD	m_dwServerID;
	BYTE	m_btChannelNO;
} ;
struct lsv_SELECT_SERVER : public t_PACKETHEADER 
{
	BYTE	m_btResult;
	DWORD	m_dwIDs[2];
	// szServerIP[];
	// WORD wServerPortNO;
} ;
#define	RESULT_SELECT_SERVER_OK					0x00
#define RESULT_SELECT_SERVER_FAILED				0x01
#define	RESULT_SELECT_SERVER_FULL				0x02	// Number kkwakcham
#define	RESULT_SELECT_SERVER_INVALID_CHANNEL	0x03	// Wrong channel selection
#define	RESULT_SELECT_SERVER_CHANNEL_NOT_ACTIVE	0x04	// Of the channel server maintenance
#define	RESUTL_SELECT_SERVER_INVALID_AGE		0x05	// It does not meet the age.
#define	RESULT_SELECT_SERVER_NEED_CHARGE		0x06	// Billing is afterwards


struct tagVAR_GLOBAL 
{
	short	m_nWorld_PRODUCT;					// Manufacturing Related
	DWORD	m_dwUpdateTIME;						// The updated time.
	short	m_nWorld_RATE;						// Economy:: the world's waterfront. 80~140
	BYTE	m_btTOWN_RATE;						// The village waterfront.					80~140
	BYTE	m_btItemRATE[ MAX_PRICE_TYPE ];		// Aitembyeol waterfront.				1~127
	DWORD	m_dwGlobalFLAGS;
} ;
struct gsv_SET_GLOBAL_VAR : public t_PACKETHEADER, public tagVAR_GLOBAL 
{
} ;

struct gsv_SET_GLOBAL_FLAG : public t_PACKETHEADER 
{
	DWORD	m_dwGlobalFLAGS;
} ;
#define	ZONE_FLAG_PK_ALLOWED		0x00000001

/*
struct cli_JOIN_WORLD : public t_PACKETHEADER 
{
	DWORD	m_dwID;
    BYTE	m_MD5Password[ 32 ];
} ;

// m_cResult 값은 lsv_LOGIN_REPLY와 같다.
struct wsv_JOIN_WORLD : public t_PACKETHEADER 
{
	BYTE	m_btResult;
	DWORD	m_dwID;
} ;
#define	RESULT_JOIN_WORLD_OK					0x00
#define	RESULT_JOIN_WORLD_FAILED				0x01
#define RESULT_JOIN_WORLD_TIME_OUT				0x02    // Should the account goes back to login timed out.
#define RESULT_JOIN_WORLD_INVALID_PASSWORD		0x03    // Off error
#define RESULT_JOIN_WORLD_ALREADY_LOGGEDIN		0x04    // Is already logged in
*/

// World/John server access request
struct cli_JOIN_SERVER_REQ	: public t_PACKETHEADER 
{
	DWORD	m_dwID;
    BYTE	m_MD5Password[ 32 ];
} ;
struct srv_JOIN_SERVER_REPLY : public t_PACKETHEADER 
{
	BYTE	m_btResult;
	DWORD	m_dwID;
	DWORD	m_dwPayFLAG;
} ;
#define	RESULT_JOIN_SERVER_OK					0x00
#define	RESULT_JOIN_SERVER_FAILED				0x01
#define RESULT_JOIN_SERVER_TIME_OUT				0x02    // Should the account goes back to login timed out.
#define RESULT_JOIN_SERVER_INVALID_PASSWORD		0x03    // Off error
#define RESULT_JOIN_SERVER_ALREADY_LOGGEDIN		0x04    // Is already logged in


// John tries to notify the client to the server moves connecting ...
struct wsv_MOVE_SERVER : public t_PACKETHEADER 
{
	WORD	m_wPortNO;
	DWORD	m_dwIDs[2];
	// szServerIP[]
} ;


struct cli_CHAR_LIST : public t_PACKETHEADER 
{
} ;


// Gender, level, occupation, part-index ...
struct tagCHARINFO 
{
	BYTE	m_btCharRACE;
	short	m_nLEVEL;
	short	m_nJOB;
	DWORD	m_dwRemainSEC;		// If non-zero when the deletion until the remaining time (in seconds)
#ifdef	__INC_PLATINUM
	BYTE	m_btIsPlatinumCHAR;	// If non-zero selection is possible only if the platinum Carrick.
#endif
} ;
struct wsv_CHAR_LIST : public t_PACKETHEADER 
{
	BYTE	m_btCharCNT;
	/*
	{
		tagCHARINFO m_CharINFO;
		tagPartITEM m_PartITEM    [ MAX_BODY_PART ];
		char		szCharName[]
	} x m_btCharCNT
	*/
} ;


struct cli_CREATE_CHAR : public t_PACKETHEADER 
{
	BYTE	m_btCharRACE;
	char	m_cBoneSTONE;
	char	m_cHairIDX;
	char	m_cFaceIDX;
	char	m_cWeaponTYPE;
	short	m_nZoneNO;
	// char szCharName[]
} ;
struct wsv_CREATE_CHAR : public t_PACKETHEADER 
{
	BYTE	m_btResult;
	BYTE	m_btIsPlatinumCHAR;
} ;
#define	RESULT_CREATE_CHAR_OK				0x00
#define	RESULT_CREATE_CHAR_FAILED			0x01
#define	RESULT_CREATE_CHAR_DUP_NAME			0x02
#define	RESULT_CREATE_CHAR_INVALID_VALUE	0x03
#define	RESULT_CREATE_CHAR_NO_MORE_SLOT		0x04	// Do not make anymore.
#define	RESULT_CREATE_CHAR_BLOCKED			0x05	// Character creation on the server, the status is prohibited.

struct cli_DELETE_CHAR : public t_PACKETHEADER 
{
	BYTE	m_btCharNO;
	bool	m_bDelete;								// true If removed, false if deleting idle characters resurrection
	// char szCharName[]
} ;
struct wsv_DELETE_CHAR : public t_PACKETHEADER 
{
	DWORD	m_dwDelRemainTIME;						// 0, undelete ...
	// char szCharName[]
} ;




struct cli_LOGOUT_REQ : public t_PACKETHEADER 
{
} ;
struct gsv_LOGOUT_REPLY : public t_PACKETHEADER 
{
	WORD	m_wWaitSec;
} ;



struct gsv_INIT_DATA : public t_PACKETHEADER 
{
	int		m_iRandomSEED;
	WORD	m_wRandomINDEX;
} ;


struct cli_SELECT_CHAR : public t_PACKETHEADER 
{
	BYTE	m_btCharNO;
	BYTE	m_btRunMODE;
	BYTE	m_btRideMODE;
	// char szCharName[]
} ;

struct gsv_SELECT_CHAR : public t_PACKETHEADER 
{
	BYTE				m_btCharRACE;
	short				m_nZoneNO;
	tPOINTF				m_PosSTART;
	short				m_nReviveZoneNO;

	tagPartITEM			m_PartITEM[ MAX_BODY_PART   ];
//	tagPartITEM			m_RideITEM[ MAX_RIDING_PART ];

	tagBasicINFO		m_BasicINFO;	
	tagBasicAbility		m_BasicAbility;	
	tagGrowAbility		m_GrowAbility;	
	tagSkillAbility		m_Skill;
	CHotICONS			m_HotICONS;
	DWORD				m_dwUniqueTAG;

#ifdef __KRRE_NEW_AVATAR_DB
	DWORD				m_dwCoolTime[MAX_SKILL_RELOAD_TYPE];
#endif

//	char				szCharName[];
} ;


struct gsv_INVENTORY_DATA : public t_PACKETHEADER 
{
	// 2004. 9.11 Replaced... CInventory	m_INV;
	struct 
	{
		__int64		m_i64Money;
		tagBaseITEM	m_ItemLIST[ INVENTORY_TOTAL_SIZE ];
	} m_INV;
} ;


// After increasing number of quests switch OFF then separated into two packets below.
struct gsv_QUEST_DATA : public t_PACKETHEADER 
{
	tagQuestData	m_Quests;
	tagWishLIST		m_WishLIST;
} ;

struct gsv_QUEST_ONLY : public t_PACKETHEADER 
{
	tagQuestData	m_Quests;
} ;
struct gsv_WISH_LIST : public t_PACKETHEADER 
{
	tagWishLIST		m_WishLIST;
} ;


struct cli_JOIN_ZONE : public t_PACKETHEADER 
{
	BYTE			m_btWeightRate;
	short			m_nPosZ;
} ;



struct gsv_JOIN_ZONE : public t_PACKETHEADER 
{
	WORD			m_wServerObjectIndex;
	short			m_nCurHP;
	short			m_nCurMP;
	long			m_lCurEXP;
	long			m_lPenalEXP;
	tagVAR_GLOBAL	m_VAR;
	DWORD			m_dwAccWorldTIME;		// World time accumulated after the first game server operation
#ifdef	__APPLY_04_10_15_TEAMNO
	int				m_iTeamNO;				// Set on the server side code :: 15.10.04 added.
#endif

} ;


struct cli_SCREEN_SHOT_TIME:public t_PACKETHEADER
{
	WORD wCnt;
};

struct gsv_SCREEN_SHOT_TIME:public t_PACKETHEADER
{
	WORD wYear;
	BYTE	btMonth;
	BYTE	btDay;
	BYTE	btHour;
	BYTE  btMin;
};

struct gsv_CHAR_HPMP_INFO:public t_PACKETHEADER			//0x07ec packet. HP and MP sent from the server. let's see if we can also pass MaxHP
{
	short m_CurHP;
	short m_CurMP;
	short m_MaxHP;		//added by PY
	short m_MaxMP;		//added by PY
};

struct gsv_CHAR_STAT_INFO:public t_PACKETHEADER			//0x07ed packet. Attack, Dodge, Def etc. sent from server		///PY: added to give server full control
{
	short m_CurAP;			//Attack Power
	short m_CurDef;			//Defense
	short m_CurAccuracy;	//Accuracy
	short m_CurDodge;		//Dodge
	short m_CurMDef;		//Magic Defense
	short m_CurCrit;		//Critical
	short m_CurMSPD;		//Move Speed
	short m_CurMaxWeight;	//Maximum weight capacity
};


struct cli_REVIVE_REQ : public t_PACKETHEADER 
{
	BYTE	m_btReviveTYPE;
} ;
#define	REVIVE_TYPE_REVIVE_POS		0x01
#define REVIVE_TYPE_SAVE_POS		0x02
#define	REVIVE_TYPE_START_POS		0x03
#define	REVIVE_TYPE_CURRENT_POS		0x04

struct gsv_REVIVE_REPLY : public t_PACKETHEADER 
{
	short	m_nZoneNO;
} ;

struct cli_SET_REVIVE_POS : public t_PACKETHEADER 
{
} ;

struct cli_SET_VAR_REQ : public t_PACKETHEADER 
{
	BYTE	m_btVarTYPE;
	int		m_iValue;
} ;
struct gsv_SET_VAR_REPLY : public cli_SET_VAR_REQ 
{
} ;
#define	REPLY_GSV_SET_VAR_FAIL_BIT	0x080 
// #define	REPLY_GSV_SET_JOB_xxx


#define	BIT_MOTION_STOP_CMD			0x080000
struct cli_SET_MOTION : public t_PACKETHEADER 
{
	short		m_nMotionNO;
	union 
	{
		WORD	m_wValue;
		struct 
		{
			WORD	m_wETC : 15;
			WORD	m_wIsSTOP :  1;
		} ;
	} ;
} ;
struct gsv_SET_MOTION : public cli_SET_MOTION 
{
	WORD		m_wObjectIDX;
} ;

struct cli_TOGGLE : public t_PACKETHEADER 
{
	BYTE	m_btTYPE;
} ;
#define	TOGGLE_TYPE_RUN		0x00
#define	TOGGLE_TYPE_SIT		0x01
#define	TOGGLE_TYPE_DRIVE	0x02
struct gsv_TOGGLE : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	BYTE	m_btTYPE;
	short	m_nRunSPEED[0];			// Pat down when riding ...
} ;

struct cli_CHAT : public t_PACKETHEADER 
{
	// szChatMSG[]
} ;
struct gsv_CHAT : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
} ;

struct cli_WHISPER : public t_PACKETHEADER 
{
	// szTargetAccount[]
	// szChatMSG[]
} ;
struct gsv_WHISPER : public t_PACKETHEADER 
{
	// szFromAccount[]
	// szChatMSG[]
} ;

struct cli_SHOUT : public cli_CHAT 
{
} ;
struct gsv_SHOUT : public t_PACKETHEADER 
{
} ;

struct cli_ALLIED_CHAT : public t_PACKETHEADER 
{
} ;
struct gsv_ALLIED_CHAT : public gsv_CHAT 
{
	int	m_iTeamNO;
	// szChatMSG[]
} ;

struct cli_ALLIED_SHOUT : public t_PACKETHEADER 
{
} ;
struct gsv_ALLIED_SHOUT : public t_PACKETHEADER 
{
	int	m_iTeamNO;
	// szFromAccount[]
	// szChatMSG[]
} ;


struct cli_PARTY_CHAT : public cli_CHAT 
{
} ;
struct gsv_PARTY_CHAT : public gsv_CHAT 
{
} ;

struct cli_CLAN_CHAT : public cli_CHAT 
{
} ;
struct wsv_CLAN_CHAT : public gsv_SHOUT 
{
} ;

struct gsv_SET_EVENT_STATUS : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	short	m_nEventSTATUS;
} ;

#define	MOVE_MODE_WALK		0x00	// Walking Mode
#define	MOVE_MODE_RUN		0x01	// Walking Mode
#define	MOVE_MODE_DRIVE		0x02	// Walking Mode : There he rides.
#define	MOVE_MODE_RIDEON	0x04	// Walking Mode : Ride to others.

struct tag_ADD_CHAR : public t_PACKETHEADER			
{
    WORD		m_wObjectIDX;
	tPOINTF		m_PosCUR;
	tPOINTF		m_PosTO;
	WORD		m_wCommand;						//CMD_STOP 0000, CMD_MOVE 0001, CMD_ATTACK 0002, CMD_DIE 0003, CMD_PICK_ITEM 0004 etc, see CObjAI.h
	WORD		m_wTargetOBJ;
	BYTE		m_btMoveMODE;					// 0:Walk, 1 run, 2: ride in my passenger Item 3: Riding is riding on items m_wTargetOBJ.
	int			m_iHP;
#ifdef	__APPLY_04_10_15_TEAMNO
	int			m_iTeamNO;						//	char m_cTeamNO => int Change to modify ... 15/10/04
#else
	char		m_cTeamNO;
#endif
	DWORD		m_dwStatusFALG;		
	/*
	Continuous skill value of the changed by ...	2004.1.4
	if ( m_dwStatusFALG & ( ING_FLAG_MAX_HP | ING_FLAG_xxx_MOV_SPEED | ING_FLAG_xxx_ATK_SPEED ) ) {
		M_nAdjSTATUS [xx] stuck behind each structure are transferred.

		BYTE btIDX = 0;
  		if ( tag_ADD_CHAR.m_dwStatusFALG &  FLAG_ING_MAX_HP ) )
			inc_max_hp = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_INC_MOV_SPEED ) )
			inc_run_spd = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_DEC_MOV_SPEED ) )
			dec_run_spd = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_INC_ATK_SPEED ) )
			inc_atk_spd = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_DEC_ATK_SPEED ) )
			dec_atk_spd = pAdjSTATUS[ btIDX++ ];

		if ( tag_ADD_CHAR.m_dwStatusFALG &	FLAG_ING_DEC_LIFE_TIME ) )		// 2004. 4. 16 추가..
			owner_obj_idx = (WORD) pAdjSTATUS[ btIDX++ ];
	}
	*/
} ;

struct gsv_MOB_CHAR : public tag_ADD_CHAR 
{
	short			m_nCharIdx;					
	short			m_nQuestIDX;
	// short		m_nAdjSTATUS[];
} ;

// gsv_NPC_CHAR In the packet m_nCharIdx <0 if hidden (not rendered on the client)
struct gsv_NPC_CHAR : public gsv_MOB_CHAR 
{
	float			m_fModelDIR;
	short			m_nEventSTATUS;				// 2004. 3. 22 Add ..., parameter values changed internally by the NPC AI.
	// short		m_nAdjSTATUS[];
} ;


/// Coal consumption is currently set
struct tagShotDATA 
{
	union 
	{
		struct 
		{
			WORD	m_cType			: 5;
			WORD	m_nItemNo		: 10;
		} ;
		WORD	m_wShotITEM;
	} ;
} ;

/*
	Calculation
	1. The attack rate = value calculated as a weapon + Passive lasting correction value correction value +
	2. Move the speed = value received from the server (including passive) + lasting correction value
*/
struct tag_CLAN_ID 
{
	DWORD	m_dwClanID;
	union 
	{
		DWORD	m_dwClanMARK;
		WORD	m_wClanMARK[2];
	} ;
	BYTE	m_btClanLEVEL;		// Clan Level
	BYTE	m_btClanPOS;		// Position in clan
} ;
struct gsv_AVT_CHAR : public tag_ADD_CHAR 
{
	BYTE		m_btCharRACE;
	short		m_nRunSpeed;						// To a value corrected by the passive, except for the value corrected by the duration.
	short		m_nPsvAtkSpeed;						// Passive ... except for the default rate value, a value corrected by the duration.
	BYTE		m_btWeightRate;						// Current carrying capacity / maximum carrying capacity * 100
	tagPartITEM m_PartITEM	  [ MAX_BODY_PART ];
	tagShotDATA	m_sShotItem	  [ MAX_SHOT_TYPE ];

	short		m_nJOB;
	BYTE		m_btLEVEL;

//	short		m_nRidingITEM [ MAX_RIDING_PART ];	// Passenger items.
	tagPartITEM	m_RidingITEM  [ MAX_RIDING_PART ];	// Passenger items.
	short		m_nPosZ;

	DWORD		m_dwSubFLAG;

	// char		szUserID[]
	// short	m_nAdjSTATUS[];
	// ( tag_ADD_CHAR.m_dwStatusFALG & FLAG_ING_STORE_MODE ) 일경우 char szStoreTITLE[] 붙음

	/*	Additional information clan ...
	tag_CLAN_ID	m_CLAN;	== NULL Or that this Clan Information
	// char m_szClanNAME[];
	*/
} ;

struct gsv_SUB_OBJECT : public t_PACKETHEADER 
{
    WORD    m_wObjectIDX[ 1 ];
} ;

struct cli_SET_WEIGHT_RATE : public t_PACKETHEADER 
{
	BYTE	m_btWeightRate;
} ;

struct gsv_SET_WEIGHT_RATE : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	BYTE	m_btWeightRate;
} ;


struct gsv_ADJUST_POS : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	tPOINTF	m_PosCUR;
	short	m_nPosZ;
} ;

struct cli_CANTMOVE : public t_PACKETHEADER 
{
    tPOINTF m_PosCUR;
	short	m_nPosZ;
} ;
struct cli_SETPOS : public t_PACKETHEADER 
{
    tPOINTF m_PosCUR;
} ;


struct cli_STOP : public t_PACKETHEADER 
{
    tPOINTF m_PosCUR;
} ;

struct gsv_STOP : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
    tPOINTF m_PosCUR;
	short	m_nPosZ;
} ;


struct cli_MOUSECMD : public t_PACKETHEADER 
{
    WORD	m_wTargetObjectIDX;
    tPOINTF m_PosTO;
	short	m_nPosZ;
} ;

struct gsv_MOUSECMD : public t_PACKETHEADER 
{
	WORD	m_wSourObjIDX;
	WORD	m_wDestObjIDX;

//	tPOINTF	m_PosCUR;
	WORD	m_wSrvDIST;
    tPOINTF m_PosTO;

	short	m_nPosZ;
} ;


struct cli_ATTACK : public t_PACKETHEADER 
{
	WORD	m_wTargetObjectIndex;
} ;

struct gsv_ATTACK : public t_PACKETHEADER 
{
	WORD	m_wAtkObjIDX;
	WORD	m_wDefObjIDX;

//	tPOINTF	m_PosCUR;
	WORD	m_wSrvDIST;
    tPOINTF m_PosTO;
} ;

struct gsv_MOVE : public gsv_MOUSECMD 
{
	BYTE	m_btMoveMODE;		// Tag_ADD_CHAR:: m_btMoveMODE the same value as the
} ;


// tag_DROPITEM . Items produced are going to die this time tteolguneun mobs set up by King Gojong name clients ITEM_OBJ LIVE_TIME.
struct tag_DROPITEM 
{
	tPOINTF		m_PosCUR;
	tagBaseITEM	m_ITEM;
	WORD		m_wServerItemIDX;
	WORD		m_wOwnerObjIDX;
	// WORD m_wRemainTIME;		// Set to delete ITEM_OBJ LIVE_TIME.
} ;
struct gsv_ADD_FIELDITEM : public t_PACKETHEADER, public tag_DROPITEM 
{
	WORD	m_wRemainTIME;
} ;

struct gsv_SUB_FIELDITEM : public t_PACKETHEADER 
{
	WORD	m_wServerItemIDX;
} ;

struct cli_GET_FIELDITEM_REQ : public t_PACKETHEADER 
{
	WORD	m_wServerItemIDX;
} ;

struct gsv_GET_FIELDITEM_REPLY : public t_PACKETHEADER 
{
	WORD		m_wServerItemIDX;
	BYTE		m_btResult;
	short		m_nInventoryListNO;
	tagBaseITEM	m_ITEM;
} ;
#define	REPLY_GET_FIELDITEM_REPLY_OK			0x00	// It was obtained.
#define REPLY_GET_FIELDITEM_REPLY_NONE			0x01	// The missing items.
#define REPLY_GET_FIELDITEM_REPLY_NO_RIGHT		0x02	// There is no authority to take ...
#define	REPLY_GET_FIELDITEM_REPLY_TOO_MANY		0x03	// I can not find any more items.


struct cli_DAMAGE : public t_PACKETHEADER 
{
	WORD	m_wDefObjIDX;
	WORD	m_wRandomIDX;
} ;


//-------------------------------------
/*
union uniDAMAGE {
		WORD	m_wDamage;
		struct {
				WORD	m_wVALUE  : 11;
				WORD	m_wACTION :  5;
		} ;
} ;
*/
// 2006.03.07/Kim Dae-Sung
// Types modified to eliminate the maximum damage 2047 (0x07ff) limited (#define MAX_DAMAGE 0x07ff)
union uniDAMAGE 
{
	__int64	m_wDamage;				// WORD => __int64 Crystal  
	struct 
	{
		DWORD	m_wVALUE;			// Split in bits instead of just writing
		DWORD	m_wACTION;			// Split in bits instead of just writing
	} ;
} ;

struct gsv_DAMAGE : public t_PACKETHEADER 
{
	WORD			m_wAtkObjIDX;
	WORD			m_wDefObjIDX;

	uniDAMAGE		m_Damage;
	tag_DROPITEM	m_DropITEM[ 0 ];	// If the value contains only one die of damage. Drop the item index
} ;

//-------------------------------------

//#define	MAX_DAMAGE			0x07ff

// 2006.03.16/Kim, Dae - Sung-corrected maximum damage - 2147483647
#define	MAX_DAMAGE			0x7FFFFFFF	
//-------------------------------------

//-------------------------------------
/*
// gsv_DAMAGE::m_wDamage Value calculation with.
#define	DMG_BIT_DUMMY		0x0800		//  I'll Further damage to attached additional damage calculation if they are hit.
#define	DMG_BIT_IMMEDIATE	0x1000		// When a packet is received and processed immediately: Yes) it demjwi reflected by damage shield skill
#define	DMG_BIT_HITTED		0x2000		// The fit operation.
#define	DMG_BIT_CRITICAL	0x4000		// The critical damage
#define	DMG_BIT_DEAD		0x8000		// died.
*/
// 2006.03.07/ Kim, Dae - Sung-corrected maximum damage - Changes in data type WORD to DWORD of uni DAMAGE
// gsv_DAMAGE::m_wDamage Value calculation with.
#define	DMG_BIT_DUMMY		0x0100000000	// I'll Further damage to attached additional damage calculation if they are hit.
#define	DMG_BIT_IMMEDIATE	0x0200000000	// When a packet is received and processed immediately: Yes) it demjwi reflected by damage shield skill
#define	DMG_BIT_HITTED		0x0400000000	// The fit operation.
#define	DMG_BIT_CRITICAL	0x0800000000	// The critical damage
#define	DMG_BIT_DEAD		0x1000000000	// died.
//-------------------------------------

// gsv_DAMAGE::m_wACTION This has value ...
#define	DMG_ACT_RESERVED	0x01
#define	DMG_ACT_ATTACK		0x02
#define	DMG_ACT_HITTED		0x04
#define	DMG_ACT_CRITICAL	0x08
#define	DMG_ACT_DEAD		0x10

struct gsv_CHANGE_NPC : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	short	m_nNPCNo;
} ;

struct cli_SUMMON_CMD : public t_PACKETHEADER 
{
	BYTE	m_btCMD;
} ;
#define	SUMMON_CMD_ATTACK		0x00
#define	SUMMON_CMD_DEFENSE		0x01
#define	SUMMON_CMD_STANDING		0x02


// To update their current experience.
struct gsv_SETEXP : public t_PACKETHEADER 
{
	long	m_lCurEXP;
	short	m_nCurSTAMINA;
	WORD	m_wFromObjIDX;
} ;

struct gsv_LEVELUP : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	short	m_nCurLevel;
	long	m_lCurEXP;
	short	m_nBonusPoint;
	short	m_nSkillPoint;
} ;


struct cli_HP_REQ : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
} ;
struct gsv_HP_REPLY : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	int		m_iHP;			// MOB hp So by sending a ...
} ;


struct gsv_SET_HPnMP : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	short	m_nHP;
	short	m_nMP;
} ;


struct tag_BUY_ITEM 
{
	char	m_cTabNO;
	char	m_cColNO;
	WORD	m_wDupCNT;		// If the number of duplicate items purchased
} ;
struct tag_SELL_ITEM 
{
	BYTE	m_btInvIDX;
	WORD	m_wDupCNT;		// If the number of duplicate items for sale
} ;
// 상점에서 거래...
struct cli_STORE_TRADE_REQ : public t_PACKETHEADER 
{
	WORD	m_wNPCObjIdx;
	char	m_cBuyCNT;
	char	m_cSellCNT;
	DWORD	m_dwEconomyTIME;
/*
	tag_BUY_ITEM	x m_cBuyCNT
	tag_SELL_ITEM	x m_cSellCNT
*/
} ;

struct gsv_STORE_TRADE_REPLY : public t_PACKETHEADER 
{
	BYTE	m_btRESULT;
} ;
#define	STORE_TRADE_RESULT_PRICE_DIFF		0x01
#define	STORE_TRADE_RESULT_NPC_NOT_FOUND	0x02
#define	STORE_TRADE_RESULT_TOO_FAR			0x03
#define	STORE_TRADE_RESULT_OUT_OF_MONEY		0x04
#define	STORE_TRADE_RESULT_NOT_UNION_USER	0x05	// Such is not the union
#define	STORE_TRADE_RESULT_OUT_OF_POINT		0x06	// Garda combination point cap
#define STORE_TRADE_RESULT_OUT_OF_REWARD_POINT	0x07	// Clan tribe reward points

// Changed inventory information ...
struct tag_SET_INVITEM 
{
	BYTE		m_btInvIDX;
	tagBaseITEM	m_ITEM;
} ;

// Changed inventory information into ...
struct gsv_SET_MONEYnINV : public t_PACKETHEADER 
{
	__int64			m_i64Money;
	BYTE			m_btItemCNT;
	tag_SET_INVITEM	m_sInvITEM[ 0 ];		// It contains as many changes ...
} ;

struct gsv_SET_INV_ONLY : public t_PACKETHEADER 
{
	BYTE			m_btItemCNT;
	tag_SET_INVITEM	m_sInvITEM[ 0 ];		// It contains as many changes ...
} ;

struct gsv_SET_MONEY_ONLY : public t_PACKETHEADER 
{
	__int64			m_i64Money;
} ;

struct gsv_SET_ABILITY : public t_PACKETHEADER 
{
	WORD	m_wAbilityTYPE;
	int		m_iValue;
} ;

#define	SERVER_DATA_ECONOMY		0
#define	SERVER_DATA_NPC			1
struct gsv_SERVER_DATA : public t_PACKETHEADER 
{
	BYTE	m_btDataTYPE;
	BYTE	m_pDATA[0];
} ;

#define	RELAY_TYPE_RECALL	0x00
struct gsv_RELAY_REQ : public t_PACKETHEADER 
{
	WORD	m_wRelayTYPE;
	union {
		BYTE	m_pBODY[1];
		struct {
			short	m_nCallZoneNO;
			tPOINTF m_PosCALL;
		} ;
	} ;
} ;
struct cli_RELAY_REPLY : public gsv_RELAY_REQ 
{
} ;

struct cli_ITEM_SKILL : public t_PACKETHEADER 
{
	short	m_nInventoryIndex;
} ;

struct cli_USE_ITEM : public t_PACKETHEADER 
{
	short	m_nInventoryIndex;
	// WORD		m_wDestObjIDX;		:: Targeted skills using a consumption item
	// tPOINTF	m_PosTARGET;		:: Location skill using a consumption item
} ;
struct gsv_USE_ITEM : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	short	m_nUseItemNO;
	BYTE	m_btInvIDX[ 0 ];
} ;

struct cli_DROP_ITEM : public t_PACKETHEADER 
{
	BYTE	m_btInventoryIndex;
	DWORD	m_uiQuantity;	
} ;


// m_nEquipInvIDX It must must contain a value.
// m_nWeaponInvIDX In the case of stripping equipment is zero!
struct cli_EQUIP_ITEM : public t_PACKETHEADER 
{
	short	m_nEquipInvIDX;		// Replacing windows inventory number :: m_nEquipInvIDX> = 1 && m_nEquipInvIDX <MAX_EQUIP_IDX
	short	m_nWeaponInvIDX;	// Equipment inventory item number is located ...
} ;


struct gsv_EQUIP_ITEM : public t_PACKETHEADER 
{
	WORD		m_wObjectIDX;
	short		m_nEquipIndex;		//Slot number
//	short		m_nItemNO;
	tagPartITEM m_PartITEM;
	/* Struct tagPartITEM
		unsigned int	m_nItemNo		: 18;	// 0~1023	Ah system number(ITEM_ID)		(0 ~ 999)
		unsigned int	m_nGEM_OP		: 9;	// 0~512	Jewelry number(m_bHasSocket == 1) Or a number of options(m_bHasSocket==0)
		unsigned int	m_bHasSocket	: 1;	// 0~1		Jewelry, whether or not a socket
		unsigned int	m_cGrade	    : 4;	// 0~15		Rating						(0~9)
	*/
	short		m_nRunSPEED[ 0 ];	// Move Speed calculated entirely by the server. Here we set it and forget it

} ;


struct cli_ASSEMBLE_RIDE_ITEM : public t_PACKETHEADER 
{
	short	m_nRidingPartIDX;	// t_eRidePART Uigap.
	short	m_nRidingInvIDX;	// cli_EQUIP_ITEM::m_nWeaponInvIDX like...
} ;

struct gsv_ASSEMBLE_RIDE_ITEM : public t_PACKETHEADER 
{
	WORD		m_wObjectIDX;
	short		m_nRidingPartIDX;
//	short		m_nItemNO;
	tagPartITEM	m_RideITEM;
	short		m_nRunSPEED[ 0 ];	// Up to the value compensated by the passive skill: When values except: Pat corrected by the Boarding continued ...
} ;


struct cli_USE_ITEM_TO_REPAIR : public t_PACKETHEADER 
{
	short	m_nUseItemInvIDX;			// Inventory number of items for repair
	short	m_nRepairTargetInvIDX;		// Inventory number of items to be repaired
} ;

struct cli_REPAIR_FROM_NPC : public t_PACKETHEADER 
{
	WORD	m_wNPCObjIDX;				// NPC index server to request repairs
	short	m_nRepairTargetInvIDX;		// Inventory number of items to be repaired
} ;


struct gsv_SET_ITEM_LIFE : public t_PACKETHEADER 
{
	short	m_nInventoryIDX;			// Inventory item number to be life changing
	short	m_nLife;					// Current life in server
} ;



// Changed inventory information ...
struct Set_MileageInv
{
	BYTE		m_btInvIDX;
	Net_MileageItem	m_ITEM;

	void Set(BYTE btIDX,MileageItem *pItem)
	{
		m_btInvIDX = btIDX;
		CopyMemory(&m_ITEM,pItem,sizeof(tagBaseITEM));
		m_ITEM.m_dwDate = pItem->m_dwDate;
	}

} ;


struct gsv_MILEAGE_INV_DATA : public t_PACKETHEADER 
{
	struct
	{
		Net_MileageItem	m_ItemLIST[ MAX_MILEAGE_INV_SIZE ];
	} m_INV;
} ;


struct gsv_DESTROY_MILEAGE_ITEM: public t_PACKETHEADER 
{
	BYTE m_btItemCNT;
	Set_MileageInv	m_sInvITEM[ 0 ];
};


typedef gsv_DESTROY_MILEAGE_ITEM gsv_NOTIFY_MILEAGE_ITEM;   	//Notification changed Mileage items
typedef gsv_DESTROY_MILEAGE_ITEM gsv_SET_MILEAGE_INV;			//Mileage inventory information notice.


typedef cli_USE_ITEM cli_USE_MILEAGE_ITEM;						//Using miles items
typedef gsv_USE_ITEM gsv_USE_MILEAGE_ITEM;

typedef cli_EQUIP_ITEM cli_EQUIP_MILEAGE_ITEM;					//Mileage wear items		
typedef gsv_EQUIP_ITEM gsv_EQUIP_MILEAGE_ITEM;


//struct cli_CHANGE_SKIN : public t_PACKETHEADER 
//{
//	BYTE	m_btBodyIDX;
//	short	m_nItemNO;
//} ;
struct gsv_CHANGE_SKIN : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	WORD	m_wAbilityTYPE;
	int		m_iValue;
} ;

struct cli_MOVE_ITEM : public t_PACKETHEADER 
{
	BYTE		m_btMoveTYPE;
	BYTE		m_btFromIDX;
	tagBaseITEM	m_MoveITEM;						// <<-- It may only be moved .. WORD m_w Quantity;
#ifdef	__INC_PLATINUM
	BYTE		m_btUseSpecialTAB;				// If the value is 1 .. Loading item has been added to the Platinum Service tab.
#endif
} ;

#define	MOVE_ITEM_TYPE_INV2BANK		0x00		// ITEM :: Inventory --> Bank
#define	MOVE_ITEM_TYPE_BANK2INV		0x01		// ITEM :: Bank --> Inventory

// If the packet size == gsv MOVEITEM warehouse => Move Inventory
// Packet size == gsv_MOVE_ITEM + sizeof (__ int64) if inventory => warehouses, m_iCurMoney [0] that the money for the
struct gsv_MOVE_ITEM : public t_PACKETHEADER 
{
	short		m_nInvIDX;						// IDX == -1 failure
	short		m_nBankIDX;						// IDX == -1 failure

	tagBaseITEM	m_InvItem;
	tagBaseITEM	m_BankITEM;
	__int64		m_iCurMoney[0];					// Results storage fee missing money ...
} ;

#define	MOVE_ZULY_TYPE_INV2BANK		0x10		// ZULY :: Inventory --> Bank
#define MOVE_ZULY_TYPE_BANK2INV		0x11		// ZULY :: Bank --> Inventory
struct cli_MOVE_ZULY : public t_PACKETHEADER 
{
	BYTE		m_btMoveTYPE;
	__int64		m_i64MoveZuly;					// Julie amount to move
} ;
struct gsv_MOVE_ZULY : public t_PACKETHEADER 
{
	__int64		m_i64InvZuly;					// The machine moves possession of a quantity Julie
	__int64		m_i64BankZuly;					// The machine moves possession of a quantity Julie
} ;


#define	BANK_REQ_OPEN				0x00		// When opening the warehouse.
#define	BANK_REQ_CHANGE_PASSWORD	0x01		// When off duty storage warehouses must be open to change ... in the state ...
struct cli_BANK_LIST_REQ : public t_PACKETHEADER 
{
	BYTE	m_btREQ;
	char	m_szPassword[0];
} ;

#define	BANK_REPLY_INIT_DATA		0x00		// Initialized data warehouse & shows
#define	BANK_REPLY_SHOW_DATA		0x01		// Warehouse show m_btItemCNT == 0
#define	BANK_REPLY_NEED_PASSWORD	0x02		// The off-duty storage needs.
#define	BANK_REPLY_CHANGED_PASSWORD	0x03		// Warehouse unbundled changes dwaeteum
#define	BANK_REPLY_INVALID_PASSWORD	0x04		// This must shed off duty
#define	BANK_REPLY_CLEARED_PASSWORD	0x05		// Deleted off-duty warehouse
#define	BANK_REPLY_PLATINUM			0x06		// Platinum warehouse contents (subsequently transferred back BANK_REPLY_INIT_DATA)
struct gsv_BANK_LIST_REPLY : public t_PACKETHEADER 
{
	BYTE			m_btREPLY;
	BYTE			m_btItemCNT;
	tag_SET_INVITEM	m_sInvITEM[ 0 ];			// For as long as the number is changed ...
} ;

struct cli_TELEPORT_REQ : public t_PACKETHEADER 
{
	short	m_nWarpIDX;
	tPOINTF	m_PosCUR;							// Hodges, the current coordinate nuclear screening: client to server coordinates and comparisons
} ;
struct gsv_TELEPORT_REPLY : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	short	m_nZoneNO;
	tPOINTF	m_PosWARP;
	BYTE	m_btRunMODE;
	BYTE	m_btRideMODE;
	//-------------------------------------
	// 2006.05.10/ Kim Dae-Seong/add-loading image warp due to the condition of a certain npc (1201) whenever such a value transfer.
	BYTE	m_pDATA[52];  //sizeof(tagObjVAR) server size : 1,	client size : 52
} ;


// m_btAbilityNO >= 100	: The index is + 100 skill points in the transmission ...
struct cli_USE_BPOINT_REQ : public t_PACKETHEADER 
{
	BYTE	m_btAbilityNO;	
} ;
struct gsv_USE_BPOINT_REPLY : public t_PACKETHEADER 
{
	BYTE	m_btAbilityNO;
	short	m_nAbilityValue;
} ;


struct cli_SET_HOTICON : public t_PACKETHEADER 
{
	BYTE		m_btListIDX;
	tagHotICON	m_HotICON;
} ;
struct gsv_SET_HOTICON : public cli_SET_HOTICON 
{
} ;


struct cli_SET_BULLET : public t_PACKETHEADER 
{
	struct 
	{
		WORD	m_wShotTYPE	: 2;
		WORD	m_wInventoryIDX : 14;
	} ;
} ;
struct gsv_SET_BULLET : public t_PACKETHEADER 
{
	WORD		m_wObjectIDX;
	tagShotDATA	m_sShot;		// m_sShot.m_wItemNO == 0 If you wrote all the bullets work!!!
} ;


struct gsv_SKILL_LEARN_REPLY : public t_PACKETHEADER 
{
	BYTE				m_btResult;
	
#ifdef __MILEAGESKILL
	WORD				m_wSkillSLOT;
#else
	BYTE				m_wSkillSLOT;
#endif

	short				m_nSkillIDX;
	short				m_nSkillPOINT;
} ;

#define	RESULT_SKILL_LEARN_FAILED			0x00		// The failure to learn.
#define	RESULT_SKILL_LEARN_SUCCESS			0x01		// Successful Withdrawal
#define	RESULT_SKILL_LEARN_NEED_JOB			0x02		// Does not match the job.
#define	RESULT_SKILL_LEARN_NEED_SKILL		0x03		// Need the skill hold.
#define	RESULT_SKILL_LEARN_NEED_ABILITY		0x04		// The lack of stats
#define	RESULT_SKILL_LEARN_OUTOFSLOT		0x05		// No longer a skill you can learn.
#define	RESULT_SKILL_LEARN_INVALID_SKILL	0x06		// The number of the wrong skills.
#define	RESULT_SKILL_LEARN_OUTOFPOINT		0x07		// Lack of skill points
#define	RESULT_SKILL_LEARN_DELETE			0x08		// 


// Skill level: ...
struct cli_SKILL_LEVELUP_REQ : public t_PACKETHEADER 
{
#ifdef __MILEAGESKILL
	WORD				m_wSkillSLOT;
#else
	BYTE				m_wSkillSLOT;
#endif
	short	m_nNextLevelSkillIDX;
} ;
struct gsv_SKILL_LEVELUP_REPLY : public t_PACKETHEADER 
{
	BYTE	m_btResult;
#ifdef __MILEAGESKILL
	WORD				m_wSkillSLOT;
#else
	BYTE				m_wSkillSLOT;
#endif
	short	m_nSkillINDEX;
	short	m_nSkillPOINT;
} ;
#define	RESULT_SKILL_LEVELUP_SUCCESS		0x00
#define	RESULT_SKILL_LEVELUP_FAILED			0x01
#define	RESULT_SKILL_LEVELUP_OUTOFPOINT		0x02
#define	RESULT_SKILL_LEVELUP_NEED_ABILITY	0x03
#define	RESULT_SKILL_LEVELUP_NEED_JOB		0x04
#define	RESULT_SKILL_LEVELUP_NEED_SKILL		0x05
#define	RESULT_SKILL_LEVELUP_OUTOFZULY		0x06		// 05.05.25 Julie may Hat


/// After applying this skill and take effect immediately: stopped
struct cli_SELF_SKILL : public t_PACKETHEADER 
{
#ifdef __MILEAGESKILL
	WORD				m_wSkillSLOT;
#else
	BYTE				m_wSkillSLOT;
#endif
} ;

struct gsv_SELF_SKILL : public t_PACKETHEADER 
{
	WORD	m_wSourObjIDX;
	short	m_nSkillIDX;
	char	cNpcSkillMOTION[ 0 ];
} ;

/// After applying the skills to apply: target stopped
struct cli_TARGET_SKILL : public t_PACKETHEADER 
{
	WORD	m_wDestObjIDX;
#ifdef __MILEAGESKILL
	WORD				m_wSkillSLOT;
#else
	BYTE				m_wSkillSLOT;
#endif
} ;

struct gsv_TARGET_SKILL : public t_PACKETHEADER 
{
	WORD	m_wSourObjIDX;
	WORD	m_wDestObjIDX;
	short	m_nSkillIDX;

	WORD	m_wSrvDIST;
	tPOINTF m_PosTO;				// Coordinate correction for 2004. 5. Add 1

	char	cNpcSkillMOTION[ 0 ];
} ;


/// Skill area: after applying stopped
struct cli_POSITION_SKILL : public t_PACKETHEADER 
{
#ifdef __MILEAGESKILL
	WORD				m_wSkillSLOT;
#else
	BYTE				m_wSkillSLOT;
#endif
	tPOINTF	m_PosTARGET;
} ;
struct gsv_POSITION_SKILL : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	short	m_nSkillIDX;
	tPOINTF	m_PosTARGET;
	char	cNpcSkillMOTION[ 0 ];
} ;

struct gsv_EFFECT_OF_SKILL : public t_PACKETHEADER 
{
	WORD			m_wObjectIDX;
	WORD			m_wSpellObjIDX;

// Hong-Keun
#ifdef _GBC
	unsigned short	m_nSkillIDX;
	unsigned short	m_nINT;
	BYTE			m_btSuccessBITS;
#else
	union 
	{
		struct 
		{
			unsigned short	m_nSkillIDX		: 12;
			unsigned short	m_btSuccessBITS : 2;	// The success of the
			BYTE			m_tmp1;
		} ;
		struct {
			BYTE			m_tmp2;
			unsigned short	m_tmp3			: 6;
			unsigned short	m_nINT			: 10;	// : E-power
		} ;
	} ;
#endif

} ;




//-------------------------------------
// Kim Dae-Sung
/*
struct gsv_DAMAGE_OF_SKILL : public gsv_EFFECT_OF_SKILL 
{
		union 
		{
			uniDAMAGE	m_Damage;				// If the value of the damage in this instance.
			WORD		m_wDamage;
		} ;
		tag_DROPITEM	m_DropITEM[ 0 ];		// Contains a value only if the damage die. The index dropped items
} ;
*/
struct gsv_DAMAGE_OF_SKILL : public gsv_EFFECT_OF_SKILL 
{
	union 
	{
		uniDAMAGE	m_Damage;				// If the value of the damage in this instance.
		__int64		m_wDamage;
	} ;
	tag_DROPITEM	m_DropITEM[ 0 ];		// Contains a value only if the damage die. The index dropped items
} ;
//-------------------------------------


struct gsv_RESULT_OF_SKILL : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	short	m_nSkillIDX;
} ;

struct gsv_SKILL_START : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
} ;

#define	SKILL_CANCEL_NEED_ABILITY		0x01	// The lack of stats
#define	SKILL_CANCEL_NEED_TARGET		0x02	// Try casting I boosted targeted ... No MP consumption
#define	SKILL_CANCEL_TARGET_NOT_FOUND	0x03	// Casting actual behavior after I boosted targeted ... MP consumption in
struct gsv_SKILL_CANCEL : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	BYTE	m_btWHY;
} ;

/*
	**	Enter a value in the packet from the server, how to become a ... AND the value declared on the DataType.h flag to the size of the order
		m_gsv_CLEAR_STATUS.m_nAdjVALUE[x]Enter a value.

	dwClearedFLAG = Client status &amp; ~ server status

	BYTE btIDX=0;	
	if ( dwClearedFLAG &  FLAG_ING_HP )
		clientHP = m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ];

	if ( dwClearedFLAG &  FLAG_ING_MP )
		clientMP = m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ];
*/
struct gsv_CLEAR_STATUS : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	DWORD	m_dwStatusFLAG;		// After the remaining flag ...
	short	m_nAdjVALUE[0];		// After calibrated values have been deleted ...
} ;

struct gsv_SPEED_CHANGED : public t_PACKETHEADER			//0x07b8	Speed change packet
{
	WORD	m_wObjectIDX;
	short	m_nRunSPEED;			// With the exception of the State, including State-passive
	short	m_nPsvAtkSPEED;			// Passive value ...
	BYTE	m_btWeightRate;			// The maximum amount of the current amount of possession/possession * 100
} ;


// Equipment commissioned: the emotions of the item: the item of emotional m_nInventoryIndex request. ..
struct cli_APPRAISAL_REQ : public t_PACKETHEADER 
{			
	WORD	m_wInventoryIndex;
} ;
struct gsv_APPRAISAL_REPLY : public t_PACKETHEADER 
{
	struct 
	{
		WORD	m_wInventoryIndex : 12;
		WORD	m_btResult : 4;
	} ;
} ;
#define	RESULT_APPRAISAL_REPLY_OK		0	// Success
#define	RESULT_APPRAISAL_REPLY_FAILED	1	// There is no money.


/// The deal sends a response to the target m_wObjectIDX.
struct cli_TRADE_P2P : public t_PACKETHEADER 
{
	BYTE	m_btRESULT;
	WORD	m_wObjectIDX;
	char	m_cTradeSLOT;
} ;
#define	RESULT_TRADE_REQUEST			0x00	// The deal required
#define	RESULT_TRADE_ACCEPT				0x01	// Trade acceptance
#define	RESULT_TRADE_REJECT				0x02	// The deal rejected
#define	RESULT_TRADE_CANCEL				0x03	// During the canceled deal
#define	RESULT_TRADE_CHECK_READY		0x04	// Ready to deal
#define	RESULT_TRADE_UNCHECK_READY		0x05	// Ready to cancel the transaction
#define	RESULT_TRADE_DONE				0x06	// Trade execution
#define	RESULT_TRADE_BUSY				0x07	// Can't do the deal.
#define	RESULT_TRADE_TOO_FAR			0x08	// A far cry from the.	Within 5m ...
#define	RESULT_TRADE_NOT_TARGET			0x09	// There is no subject.
#define	RESULT_TRADE_OUT_OF_INV			0x0a	// The other is the lack of inventory.
#define	RESULT_TRADE_NO_CHARGE_TARGET	0x0b

/// In response to the m_wObjectIDX of the deal sent from the server.
struct gsv_TRADE_P2P : public t_PACKETHEADER 
{
	BYTE	m_btRESULT;
	WORD	m_wObjectIDX;
	char	m_cTradeSLOT;
} ;


#define	MAX_TRADE_ITEM_SLOT		11
#define	TRADE_MONEY_SLOT_NO		(MAX_TRADE_ITEM_SLOT-1)
struct cli_TRADE_P2P_ITEM : public t_PACKETHEADER 
{
	char	m_cTradeSLOT;
	short	m_nInventoryIndex;				// Inventory number
	DWORD	m_uiQuantity;					// Quantity
} ;
struct gsv_TRADE_P2P_ITEM : public t_PACKETHEADER 
{
	char		m_cTradeSLOT;
	tagBaseITEM	m_ITEM;
} ;


struct cli_SET_WISHITEM : public t_PACKETHEADER 
{
	BYTE		m_btWishSLOT;
	tagBaseITEM	m_ITEM;
} ;

// Private shop related ....
#define	MAX_P_STORE_ITEM_SLOT	31

// Sell your item
struct tagPS_ITEM 
{
	union 
	{
		BYTE	m_btInvIDX;						// For sale: Ben No.
		BYTE	m_btWishIDX;					// Purchase WishList slot number
	} ;
	tagBaseITEM	m_ITEM;							// Items (if you sell an item number, the number of duplicates)
	DWORD		m_dwPricePerEA;					// 1 Selling price per piece
} ;

#define	STORE_MODE_TO_SELL		0x01
#define	STORE_MODE_TO_BUY		0x02
struct cli_P_STORE_OPEN : public t_PACKETHEADER 
{
	BYTE	m_btSellItemCNT;	// I hope the sale item registration number
	BYTE	m_btWishItemCNT;	// I hope to buy the item number of the registration

	// The item register
	tagPS_ITEM	m_ITEMs[ 0 ];	// For sale 3 0-2 2, buy, sell, and hope the purchase items 3 and 4
	// char	m_szStoreTITLE[0]
} ;
struct gsv_P_STORE_OPENED : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	char	m_szStoreTITLE[0];
} ;


struct cli_P_STORE_CLOSE : public t_PACKETHEADER 
{
} ;
struct gsv_P_STORE_CLOSED : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
} ;


struct tagPS_SLOT_PRICE 
{
	BYTE		m_btSLOT;
	tagBaseITEM	m_SlotITEM;
	DWORD		m_dwPRICE;
} ;
struct cli_P_STORE_LIST_REQ : public t_PACKETHEADER 
{		// Ask your store items list
	WORD				m_wStoreObjectIDX;
} ;
struct gsv_P_STORE_LIST_REPLY : public t_PACKETHEADER 
{
	BYTE				m_btSellItemCNT;
	BYTE				m_btWishItemCNT;
	tagPS_SLOT_PRICE	m_SlotITEMs[0];
} ;


struct tagPS_SLOT_ITEM 
{
	BYTE		m_btSLOT;
	tagBaseITEM	m_SlotITEM;
} ;
// Private shop purchase request
struct cli_P_STORE_BUY_REQ : public t_PACKETHEADER 
{
	WORD				m_wStoreObjectIDX;
	BYTE				m_btItemCNT;
	tagPS_SLOT_ITEM		m_BuyITEMs[0];
} ;

struct tagSELL_ITEM
{
		BYTE		m_btInvIDX;			// Sell item to inventory number located
		BYTE		m_btWishSLOT;		// The number of the item you want, shop owner located
		tagBaseITEM	m_SellITEM;			// Items to sell (in the case of a number of the duplicate count items sell adjustable)
};
// Ask your store sales
struct cli_P_STORE_SELL_REQ : public t_PACKETHEADER 
{
	WORD			m_wStoreObjectIDX;
	BYTE			m_btItemCNT;
	tagSELL_ITEM	m_SellITEMs[0];
} ;

struct gsv_P_STORE_RESULT : public t_PACKETHEADER 
{
	WORD			m_wStoreObjectIDX;
	BYTE			m_btResult;
	BYTE			m_btItemCNT;
	tagPS_SLOT_ITEM m_UpdatedITEM[0];
} ;
#define	RESULT_P_STORE_CANCLED				0x01	// Transaction cancellation,
#define	RESULT_P_STORE_TRADE_SOLD_OUT		0x02	// Goods out of stock
#define	RESULT_P_STORE_TRADE_NO_MORE_NEED	0x03	// Shop owner sold the desired item to someone first.
#define RESULT_P_STORE_BOUGHT_PART			0x04	// Purchase complete ... However, some items are out of stock failed to buy ...
#define RESULT_P_STORE_BOUGHT_ALL			0x05	// Complete the entire purchase the requested item. 
#define RESULT_P_STORE_SOLD_PART			0x06	// For sale complete ... However, some items will be sold after the failure to buy ...
#define RESULT_P_STORE_SOLD_ALL				0x07	// The requested item, the entire sale is complete

/*
	party structure ...
*/
struct cli_PARTY_REQ : public t_PACKETHEADER 
{
	BYTE	m_btREQUEST;
	DWORD	m_dwDestIDXorTAG;
} ;
struct gsv_PARTY_REQ : public t_PACKETHEADER 
{
	BYTE	m_btREQUEST;
	DWORD	m_dwFromIDXorTAG;
//	short	m_nZoneNO;
} ;
#define	PARTY_REQ_MAKE				0x00	// Forming a party request 
#define	PARTY_REQ_JOIN				0x01	// Party registration
#define	PARTY_REQ_LEFT				0x02	// Party withdrawal requests
#define	PARTY_REQ_CHANGE_OWNER		0x03	// Wave Rock change request.

#define	PARTY_REQ_BAN				0x81	// The party dropped out of the River on request

struct cli_PARTY_REPLY : public t_PACKETHEADER 
{
	BYTE	m_btREPLY;
	DWORD	m_dwDestIDXorTAG;
} ;
struct gsv_PARTY_REPLY : public t_PACKETHEADER 
{
	BYTE	m_btREPLY;
	DWORD	m_dwFromIDXorTAG;
} ;
#define	PARTY_REPLY_NOT_FOUND				0x00	// Did not find the target.
#define	PARTY_REPLY_BUSY					0x01	// The target cannot participate in the State party.
#define	PARTY_REPLY_ACCEPT_MAKE				0x02
#define	PARTY_REPLY_ACCEPT_JOIN				0x03
#define	PARTY_REPLY_REJECT_JOIN				0x04	// The party refused to attend the formation/...
#define	PARTY_REPLY_DESTROY					0x05	// The party has been eating out
#define PARTY_REPLY_FULL_MEMBERS			0x06	// It is the only land in the garden.
#define	PARTY_REPLY_INVALID_LEVEL			0x07	// Level does not
#define	PARTY_REPLY_CHANGE_OWNER			0x08
#define	PARTY_REPLY_CHANGE_OWNERnDISCONN	0x09	// PARTY_REPLY_CHANGE_OWNER Are as follows ... The only changes the State of an existing wave-CHAN to disconnect.
#define	PAATY_REPLY_NO_CHARGE_TARGET		0x0a

#define	PARTY_REPLY_BAN						0x80
#define PARTY_REPLY_DISCONNECT				0x81	// Abnormal termination ...
#define	PARTY_REPLY_REJOIN					0x82	// Abnormal termination after the fold.

/*
struct tag_PARTY_USER 
{		// Sent by the server in the world
	WORD	m_wUserTAG;
	//char	m_szCharName[]
} ;
struct wsv_PARTY_USER : public t_PACKETHEADER 
{
	char	m_cUserCNT;					// -1, the party in the House of representatives, the number of users that will be added, with the exception of the m_cType ...
	WORD	m_wUserTAG[0];				// If the party except circle, ...
//	tag_PARTY_USER[ m_cUserCNT ];		// If you add a reference to the party House
} ;
*/
struct tag_PARTY_MEMBER // John sent by the server
{	
	DWORD	m_dwUserTAG;
	WORD	m_wObjectIDX;
	short	m_nMaxHP;
	short	m_nHP;
	DWORD	m_dwStatusFALG;

	short	m_nCON;						// BYTE	m_btCON; //Increasing the maximum range of 300 stats as of 2004. 7.21

	BYTE	m_btRecoverHP;				// item_recover_hp + passive_recover_hp
	BYTE	m_btRecoverMP;				// item_recover_mp + passive_recover_mp

	short	m_nSTAMINA;

	//char	m_szCharName[];				// ** PARTY_MEMBER_ADD Only when the value contains
} ;
struct gsv_PARTY_MEMBER : public t_PACKETHEADER 
{
	BYTE	m_btPartyRULE;				// Party rules (item distribution, distribution of experience)
	char	m_cUserCNT;					// -1 indicates that the party with the exception of the House of representatives, the number of users that will be added, positive m_cType is ObjTAG ...
	DWORD	m_dwObjectTAG[0];			// If the party except circle, ...
	/*
	tag_PARTY_MEMBER[ m_cUserCNT ];		// If you add a reference to the party House
	*/
} ;
#define	PARTY_MEMBER_SUB	(-1)

struct gsv_PARTY_LEVnEXP : public t_PACKETHEADER 
{
	BYTE				m_btLEVEL;
	struct 
	{
		unsigned int	m_iEXP		 : 31;
		unsigned int	m_bitLevelUP : 1;
	} ;
} ;

struct gsv_CHANGE_OBJIDX : public t_PACKETHEADER 
{
	tag_PARTY_MEMBER	m_Member;
} ;

struct gsv_PARTY_ITEM : public t_PACKETHEADER 
{
	WORD		m_wObjectIDX;
	tagBaseITEM m_ITEM;
} ;

#define	BIT_PARTY_RULE_EXP_PER_PLAYER		0x001	// Take share proportionally to the level of experience ...
#define	BIT_PARTY_RULE_ITEM_TO_ORDER		0x080	// Given priority in the order ...
struct cli_PARTY_RULE : public t_PACKETHEADER 
{
	BYTE	m_btPartyRUEL;
} ;
struct gsv_PARTY_RULE : public t_PACKETHEADER 
{
	BYTE	m_btPartyRULE;
} ;


struct gsv_ADD_EVENTOBJ : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	char	m_cMapX;
	char	m_cMapY;
	WORD	m_wMapEventID;
	short	m_nEventSTATUS;
} ;
/*
struct gsv_SUB_EVENTOBJ : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
} ;
*/

struct gsv_SET_NPC_SHOW : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	bool	m_bShow;						// m_bShow == true F a l s e visibility/behind, hidden
} ;


//--------------------------- CLAN Related packet
#define	GCMD_CREATE				0x000		//Guild creation,/guildcreate [Guild people]
#define	GCMD_INFO				0x001		//Guild info,/ginfo-basic information about Guild
#define	GCMD_INVITE				0x002		//Guild invitations,/ginvite is equivalent to the players-Guild < player > invite
#define	GCMD_REMOVE				0x003		//The Guild exile,/gremove the players-Guild < player > kicking
#define	GCMD_PROMOTE			0x004		//Guild upgrades,/gpromote and raise the players Guild rating < player >-
#define	GCMD_DEMOTE				0x005		//Relegated to the Guild, the Guild rating < player >/gdemote-down
#define	GCMD_MOTD				0x006		//The Guild bulletin,/gmotd set today's Guild message < say >-
#define	GCMD_QUIT				0x007		//To unsubscribe from opt-out,/gquit-Guild the Guild
#define	GCMD_ROSTER				0x008		//View a list of Guild and the Guild list,/groster-full
#define	GCMD_LEADER				0x009		//The Guild delegation,/gleader other players Guild < player >-Chapter delegate (Guild chapter only)
#define	GCMD_DISBAND			0x00a		//The Guild disbanded,/gdisband-Guild disbanded (Guild chapter only) 

#define GCMD_INVITE_REQ			0x00b		//I was asked to join a clan. < Requestor name >
#define	GCMD_INVITE_REPLY_YES	0x00c		//Clan join acceptance < requestor name >
#define	GCMD_INVITE_REPLY_NO	0x00d		//Clancy refused to join < requestor name >

#define	GCMD_SLOGAN				0x00e		//Guild/clan slotan slogan slogan

#define	GCMD_MEMBER_JOBnLEV		0x00f		//Their job is to notify other members/level (after login/level up after/I, immediately after the transfer)

//-- Dedicated server type ...
#define	GCMD_LOGIN				0x0f0
#define	GCMD_LOGOUT				0x0f1
#define	GCMD_CHAT				0x0f2
#define	GCMD_ADJ_VAR			0x0f3
#define	GCMD_CLANMARK_SET		0x0f4
#define	GCMD_CLANMARK_GET		0x0f5
#define GCMD_CLANMARK_REGTIME	0x0f6


struct cli_CLAN_COMMAND : public t_PACKETHEADER 
{
	BYTE	m_btCMD;
} ;
struct cli_CLAN_CREATE : public cli_CLAN_COMMAND 
{
	WORD	m_wMarkIDX[2];
	// char	m_szName[];
	// char m_szDesc[];
} ;
struct cli_CLAN_MEMBER_JOBnLEV : public cli_CLAN_COMMAND 
{
	short	m_nLEVEL;
	short	m_nJOB;
} ;
struct wsv_CLAN_MEMBER_JOBnLEV : public t_PACKETHEADER 
{
	BYTE	m_btRESULT;
	short	m_nLEVEL;
	short	m_nJOB;
	// char m_szCharNAME[]
} ;

//struct tag_CLAN_INFO 
//{
//	short		m_nClanLEVEL;
//	int			m_iClanSCORE;
//	int			m_iClanRATE;	// Window efficiency
//	__int64		m_biClanMONEY;
//	char szName[];				Name
//	char szDesc[];				The slogan does not exist, NULL
//	char szAlliedGroup[]		The Alliance does not have a name for the NULL
//} ;

#define	MAX_CLAN_SKILL_SLOT		20
#ifdef	MAX_CLAN_SKILL_SLOT
	struct tagClanSKILL 
	{
		short	m_nSkillIDX;
		DWORD	m_dwExpiredAbsSEC;	// Use the expiration time
	//	short	m_nCount;			// The number of available-the number of available?? who > Master only??
	} ;
	struct tagClanBIN 
	{
		union 
		{
			BYTE			m_pDATA[ 1 ];
			tagClanSKILL	m_SKILL[ MAX_CLAN_SKILL_SLOT ];
		} ;
	} ;

#endif

struct tag_MY_CLAN : public tag_CLAN_ID 
{
	int				m_iClanSCORE;		// Clan point
	int				m_iClanRATE;		// Window efficiency
	__int64			m_biClanMONEY;
	short			m_nMemberCNT;		// The number of members
	int				m_iClanCONT;		// Clan contributions
#ifdef	MAX_CLAN_SKILL_SLOT
	tagClanBIN		m_ClanBIN;			// Clan binary data ...
#endif

#ifdef __CLAN_INFO_ADD
	//          Clancy added parameter setting.        Azadeh: setting.
	//          2006 November 22, 2009
	int             m_iRankPoint;       // Rank points.


#ifdef __EDIT_CLANDLG_CLANWAR
	// (20061222:Nam Byung Chul): clan I add reward points
	int				m_iRewardPoint;
#endif
	

#endif

	// char m_szClanName[];
	// char m_szClanDESC[];
	// char m_szClanMOTD[];
} ;

struct tag_CLAN_MEMBER 
{
	BYTE	m_btClanPOS;
	BYTE	m_btChannelNO;		// A value of 0xff indicates that the user does not access the [...]
	int		m_iClanCONTRIBUTE;
	short	m_nLEVEL;
	short	m_nJOB;
	// char m_szCharNAME[]
} ;
struct wsv_CLAN_COMMAND : public t_PACKETHEADER 
{
	BYTE	m_btRESULT;
	char	m_szStr[0];

	/*
	// m_btRESULT == RESULT_CLAN_MY_DATA
		tag_MY_CLAN	m_CLAN;
		// char m_szClanNAME[];
	*/

	/*
	// m_btRESULT == RESULT_CLAN_INFO 일If
	struct tag_CLAN_INFO
	*/
	/*
	// m_btRESULT == RESULT_CLAN_ROSTER If you are a
	tag_CLAN_MEMBER	m_Members[ x ];
	*/
	/*
	// m_btRESULT == RESULT_CLAN_KICK, RESULT_CLAN_POSITION If you are a
	strujct {
		char *m_szMember;	Unsubscribe now
		char *m_szMaster;	If NULL == sleep that withdrawal, outgoing themselves
	*/
	/*
	// m_btRESULT == RESULT_CLAN_JOINED
	struct {
		char *m_szMember;	New subscribers
		char *m_szMaster;	Join now
	} ;
	*/
	/*
	// m_btRESULT == RESULT_CLAN_SET :: Set the user's around the clan information
	struct {
		WORD		m_wObjIDX;
		tag_CLAN_ID	;
	} ;
	*/
} ;
struct wsv_RESULT_CLAN_SET : public t_PACKETHEADER 
{
	BYTE		m_btRESULT;
	WORD		m_wObjIDX;
	tag_CLAN_ID	m_ClanID;
} ;

#define	RESULT_CLAN_CREATE_OK			0x030		// I had founded.
#define RESULT_CLAN_JOIN_OK				0x031		// I was not registered?
#define	RESULT_CLAN_DESTROY_OK			0x032
#define RESULT_CLAN_MY_DATA				0x033		// If you have already joined clan is my clan Info
#define	RESULT_CLAN_MOTD				0x034		// Clan notice.
#define	RESULT_CLAN_SET					0x035		// Set the user's around the clan information
#define	RESULT_CLAN_SLOGAN				0x036		// Clan slogan has been changed

#define	RESULT_CLAN_CREATE_FAILED		0x041
#define	RESULT_CLAN_CREATE_DUP_NAME		0x042
#define	RESULT_CLAN_CREATE_NO_RIGHT		0x043		// Established rights.
#define	RESULT_CLAN_CREATE_NO_CONDITION	0x044		// Established condition.

#define	RESULT_CLAN_DESTROYED			0x051		// Clancy has been eating out ...
#define	RESULT_CLAN_DESTROY_FAILED		0x052
#define	RESULT_CLAN_DESTROY_NO_RIGHT	0x053		// Park a gallon, there is no authority.

#define	RESULT_CLAN_JOIN_MEMBER			0x061		// New subscription:: ex-Klan won me a transfer ...
#define	RESULT_CLAN_JOIN_FAILED			0x062
#define	RESULT_CLAN_JOIN_NO_RIGHT		0x063		// There is no authority to sign up.
#define	RESULT_CLAN_JOIN_HAS_CLAN		0x064		// Join other guilds.
#define	RESULT_CLAN_JOIN_MEMBER_FULL	0x065		// Full members.

#define	RESULT_CLAN_INFO				0x071		// Clan information
#define	RESULT_CLAN_ROSTER				0x072		// Clan list

#define	RESULT_CLAN_MEMBER_LOGIN		0x073		// Total sent to Member login
#define	RESULT_CLAN_MEMBER_LOGOUT		0x074		// Total members sent to log out
#define	RESULT_CLAN_MEMBER_POSITION		0x075		// Adjust the position of the members sent to the entire

#define	RESULT_CLAN_KICK				0x081		// Clan banished from as
#define	RESULT_CLAN_QUIT				0x082		// Deleted from the clan of belonging (clan variable initialization)
#define	RESULT_CLAN_POSITION			0x083		// Clancy was a rating of tuning in.
#define	RESULT_CLAN_MEMBER_JOBnLEV		0x084		// The level of the Member/job logging


// m_dwClanID The request to the server, the clan of the mark
struct cli_CLANMARK_REQ	: public t_PACKETHEADER 
{
	DWORD	m_dwClanID;
} ;
// m_btMARK Data set as their clan
struct cli_CLANMARK_SET : public t_PACKETHEADER 
{
	WORD	m_wMarkCRC16;
//	BYTE	m_btMARK[ 0 ];
} ;
// m_dwClanID Clancy's clan mark data
#define	RESULT_CLANMARK_TOO_MANY_UPDATE		0x0001	// Mark the update should be in after a period of time ...
#define	RESULT_CLANMARK_DB_ERROR			0x0002	// D. non-renewal errors
#define	RESULT_CLANMAKR_SP_ERROR			0x0003	// D. non-SP errors
struct wsv_CLANMARK_REPLY : public t_PACKETHEADER 
{
	DWORD		m_dwClanID;
	union 
	{
		WORD	m_wMarkCRC16;
		WORD	m_wFailedReason;	// m_dwClanID == 0 If you fail in the cause.
	} ;
//	BYTE	m_btMARK[ 0 ];
} ;

struct cli_CLANMARK_REG_TIME : public t_PACKETHEADER 
{
} ;
struct wsv_CLANMARK_REG_TIME : public t_PACKETHEADER 
{
	WORD	m_wYear;
	BYTE	m_btMon;
	BYTE	m_btDay;
	BYTE	m_btHour;
	BYTE	m_btMin;
	BYTE	m_btSec;
} ;

struct gsv_CHEAT_CODE : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	char	m_szCheatCODE[0];
} ;


/*
	Manufacturing-related items ...
*/
#define	CREATE_ITEM_STEP	4
struct cli_CREATE_ITEM_REQ : public t_PACKETHEADER 
{
#ifdef __MILEAGESKILL
	WORD				m_wSkillSLOT; // Use manufacturing skills
#else
	BYTE				m_wSkillSLOT;
#endif
	char	m_cTargetItemTYPE;						// The manufacture of the item type (the value of t_eITEM)
	short	m_nTargetItemNO;						// Manufacturing item number
	short	m_nUseItemINV[ CREATE_ITEM_STEP ];		// The factory-use items the inventory number
//	short	m_nUseItemCNT[ CREATE_ITEM_STEP ];		// Number of items use the factory
} ;
struct gsv_CREATE_ITEM_REPLY : public t_PACKETHEADER 
{
	BYTE		m_btRESULT;								// The result ...
	short		m_nStepORInvIDX;						// Manufacturing went a step failed, successful failure: the inventory number
	short		m_nPRO_POINT[ CREATE_ITEM_STEP ];		// Progress point
	tagBaseITEM	m_CreatedITEM;							// Manufacturing success: manufactured items
} ;
#define	RESULT_CREATE_ITEM_SUCCESS				0x00	// Success
#define	RESULT_CREATE_ITEM_FAILED				0x01	// Failure
#define	RESULT_CREATE_ITEM_INVALID_CONDITION	0x02	// Mana is scarce.
#define	RESULT_CREATE_ITEM_NEED_ITEM			0x03	// The lack of an item be consumed
#define	RESULT_CREATE_ITEM_INVALID_ITEM			0x04	// Be consumed items is invalid
#define	RESULT_CREATE_ITEM_NEED_SKILL_LEV		0x05	// Manufacturing skill level is insufficient

//struct cli_CREATE_ITEM_EXP_REQ : public t_PACKETHEADER 
//{
//} ;

#define	REPORT_ITEM_CREATE_START				0x00	// Started the manufacture of the item
#define	REPORT_ITEM_CREATE_SUCCESS				0x01	// Item manufacturing success
#define	REPORT_ITEM_CREATE_FAILED				0x02	// Item manufacturing failures
#define	REPORT_ITEM_UPGRADE_START				0x03	// Item No. started drilling
#define	REPORT_ITEM_UPGRADE_SUCCESS				0x04	// Item No. drill success
#define	REPORT_ITEM_UPGRADE_FAILED				0x05	// Item smelter failed

struct cli_ITEM_RESULT_REPORT : public t_PACKETHEADER 
{
	BYTE	m_btREPORT;
	BYTE	m_btItemType;
	short	m_nItemNO;
} ;
struct gsv_ITEM_RESULT_REPORT : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	BYTE	m_btREPORT;
	BYTE	m_btItemType;
	short	m_nItemNO;
} ;

//---------------------------------------------------------
#define	CRAFT_GEMMING_REQ			0x01			// Re-Ming
#define CRAFT_BREAKUP_USE_SKILL		0x02			// Broken down into skills
#define CRAFT_BREAKUP_FROM_NPC		0x03			// Npc through decomposition
#define	CRAFT_UPGRADE_USE_SKILL		0x04			// As a skill drill
#define	CRAFT_UPGRADE_FROM_NPC		0x05			// Npc through Jae-ryeon
#define CRAFT_DRILL_SOCKET_FROM_ITEM		0x06	// Create a socket with the use of the drill items. ..
#define CRAFT_DRILL_SOCKET_FROM_NPC		0x07		// Create a socket with the use of the drill items. ..


struct cli_CRAFT_ITEM_REQ : public t_PACKETHEADER 
{
	BYTE	m_btTYPE;
} ;
struct cli_CRAFT_GEMMING_REQ : public cli_CRAFT_ITEM_REQ 
{
	BYTE	m_btEquipInvIDX;						// The weapon item equipped with Ben No.
	BYTE	m_btJemInvIDX;							// A number of the jewellery item Ben
} ;
struct cli_CRAFT_BREAKUP_REQ : public cli_CRAFT_ITEM_REQ 
{
	short	m_nSkillSLOTorNpcIDX;					// Use the slot number or the skill npc number
	BYTE	m_btTargetInvIDX;						// The number of the item that you want to detach
} ;
#define	UPGRADE_ITEM_STEP		3
struct cli_CRAFT_UPGRADE_REQ : public cli_CRAFT_ITEM_REQ 
{
	short	m_nSkillSLOTorNpcIDX;					// Use the slot number or the skill npc number
	BYTE	m_btTargetInvIDX;						// Re do item drilling Ben No.
	BYTE	m_btUseItemINV[ UPGRADE_ITEM_STEP ];	// Ash when the item inventory number use
} ;


struct cli_CRAFT_DRILL_SOCKET_REQ : public cli_CRAFT_ITEM_REQ 
{
	short	m_nUseInvSLOTorNpcIDX;			//Npc number drill items use Ben number or
	BYTE	m_btTargetInvIDX;						//The number of the target item, Ben
} ;



#define	CRAFT_GEMMING_SUCCESS		0x01
#define	CRAFT_GEMMING_NEED_SOCKET	0x02	// Socket.
#define	CRAFT_GEMMING_USED_SOCKET	0x03	// If the re-dimming etc. ...

#define	CRAFT_BREAKUP_SUCCESS_GEM	0x04	// Jewelry separate success
#define	CRAFT_BREAKUP_DEGRADE_GEM	0x05	// Jewelry separate success, gem-grade demotion.
#define	CRAFT_BREAKUP_CLEARED_GEM	0x06	// Jewelry separate success, jewelry has been cleared.

#define	CRAFE_BREAKUP_SUCCESS		0x07	// Items disassembled success
#define CRAFE_INVALID_ITEM				0x08 //Invalid item
#define CRAFE_NOT_ENOUGH_MONEY		0x09  //Don insufficiency
#define CRAFE_NOT_ENOUGH_MP             0x0A  //MP Insufficiency


#define	CRAFT_UPGRADE_SUCCESS		0x10	// Re drilling success
#define	CRAFT_UPGRADE_FAILED		0x11	// Re drill fail
#define	CRAFT_UPGRADE_INVALID_MAT	0x12	// Material items was wrong.
#define CRAFT_GIFTBOX_SUCCESS		0X13	// Gift box decomposition

//Socket drill system. ..                   Heo Jae-Young (2005.10.27)
#define CRAFT_DRILL_SOCKET_SUCCESS	0x20    //Small cat creation success
#define CRAFT_DRILL_CLEAR_USEITME      0x21    // Drill items disappeared ...
#define CRAFE_DRILL_INVALID_USEITEM   0x22    //Drill items.
#define	CRAFT_DRILL_EXIST_SOCKET	        0x23	  // The socket is already in.
#define	CRAFT_DRILL_INVALID_TITEM	    0x24	  //Sockets, impenetrable items ...

struct tmp_InvItem
{
	byte py_Slot;
	short py_ItemType;
	short py_ItemNum;
	byte py_ItemCnt;
};

struct gsv_CRAFT_ITEM_REPLY : public t_PACKETHEADER 
{
	BYTE			m_btRESULT;
	BYTE			m_btOutCNT;			// Change the number of items
	//tag_SET_INVITEM	m_sInvITEM[ 0 ];	// For as long as the number is changed ... 
										// CRAFT_UPGRADE_SUCCESS, CRAFT_UPGRADE_FAILED, exception) if
										// m_sInvITEM[ m_btOutCNT-1 ].m_iQuantity It also contains the calculated value of the success in the
	//PY: trying this instead. The above code doesn't seem to transfer the itemtype and number
	tmp_InvItem		m_sInvITEM[ 0 ];
} ;


/*
///	Quest related ...
*/
struct cli_QUEST_REQ : public t_PACKETHEADER 
{
	BYTE	m_btTYPE;
	BYTE	m_btQuestSLOT;
	union 
	{ 
		int			m_iQuestID;
		t_HASHKEY	m_TriggerHash;
	} ;
} ;


#define	TYPE_QUEST_REQ_ADD						0x01
#define	TYPE_QUEST_REQ_DEL						0x02
#define	TYPE_QUEST_REQ_DO_TRIGGER				0x03


struct gsv_QUEST_REPLY : public t_PACKETHEADER 
{
	BYTE	m_btResult;
	BYTE	m_btQuestSLOT;
	union 
	{
		int			m_iQuestID;
		t_HASHKEY	m_TriggerHash;
	} ;
} ;
#define	RESULT_QUEST_REPLY_ADD_SUCCESS			0x01
#define	RESULT_QUEST_REPLY_ADD_FAILED			0x02
#define	RESULT_QUEST_REPLY_DEL_SUCCESS			0x03
#define	RESULT_QUEST_REPLY_DEL_FAILED			0x04
#define	RESULT_QUEST_REPLY_TRIGGER_SUCCESS		0x05		// This packet is received, the client applies the compensation.
#define	RESULT_QUEST_REPLY_TRIGGER_FAILED		0x06

// The server to the client, check the events in the NPC have and voila ....
struct gsv_CHECK_NPC_EVENT : public t_PACKETHEADER 
{
	short	m_nNpcIDX;
} ;


//--------------------------- Messenger Related packet
// Packet types are CLI_MESSENGER, m_btCMD, WSV_MESSENGER with type determined by the packet structure.
#define	FRIEND_STATUS_NORMAL		0x000		// General
#define FRIEND_STATUS_HUNT			0x001		// During the hunt
#define	FRIEND_STATUS_STORE			0x002		// Among the deals
#define	FRIEND_STATUS_QUEST			0x003		// Quest of the
#define	FRIEND_STATUS_EAT			0x004		// Table
#define	FRIEND_STATUS_REST			0x005		// During a break
#define	FRIEND_STATUS_ONLINE		0x007		// Access: m_dwUserTAG is in the list, access your friends.
#define	FRIEND_STATUS_OFFLINE		0x008		// Disconnection: m_dwUserTAG of the list is a friend of disconnection
#define	FRIEND_STATUS_REFUSE_MSG	0x009		// All the refuse message

#define	FRIEND_STATUS_REFUSED		0x080		// My conversation was blocked
#define	FRIEND_STATUS_DELETED		0x081		// I had to delete it from the list of friends


#define	MSGR_CMD_APPEND_REQ			0x001		// The target must be request: Add friends connect
#define MSGR_CMD_APPEND_ACCEPT		0x002		// Add friend request acceptance
#define	MSGR_CMD_APPEND_REJECT		0x003		// Refuse the request to add a friend
#define	MSGR_CMD_NOT_FOUND			0x004		// Failed to add a friend (if the target is access etc. ..)
#define	MSGR_CMD_DELETE				0x005		// Delete: delete the m_dwUserTAG from the list of friends and. ..
#define	MSGR_CMD_LIST				0x006		// Friends list: ask your friends list
#define	MSGR_CMD_REFUSE				0x007		// Block messages: a message of rejection from the list of m_dwUserTAG
#define	MSGR_CMD_CHANGE_STATUS		0x008		// Status change

#define	MSGR_CMD_LOGIN				0x080		// Use only on the server
#define	MSGR_CMD_LOGOUT				0x081		// Use only on the server


struct tag_MCMD_HEADER : public t_PACKETHEADER 
{
	BYTE	m_btCMD;
} ;
struct cli_MCMD_APPEND_REQ : public tag_MCMD_HEADER 
{		// Add friend requests
	char	m_szName[0];
} ;
struct cli_MCMD_APPEND_REPLY : public tag_MCMD_HEADER 
{		// It is a response to a request to add a friend
	WORD	m_wUserIDX;
} ;
struct cli_MCMD_TAG : public tag_MCMD_HEADER 
{				// Delete, block requests
	DWORD	m_dwUserTAG;
} ;
struct cli_MCMD_STATUS_REQ : public tag_MCMD_HEADER 
{		// My status change request
	BYTE	m_btStatus;
} ;


struct wsv_MCMD_APPEND_REQ : public tag_MCMD_HEADER 
{		// When I received a request to add a friend
	WORD	m_wUserIDX;
	char	m_szName[0];
} ;
struct wsv_MCMD_APPEND_ACCEPT : public tag_MCMD_HEADER 
{	// If you add it to the list of additional consent, friends
	DWORD	m_dwUserTAG;
	BYTE	m_btStatus;
	char	m_szName[0];
} ;
struct wsv_MCMD_STATUS_REPLY : public tag_MCMD_HEADER 
{		// Server response from: m_dwUserTAG state change notification
	DWORD	m_dwUserTAG;
	BYTE	m_btStatus;
} ;

struct wsv_MCMD_LIST : public tag_MCMD_HEADER 
{			// Friends list
	BYTE	m_btFriendCNT;
	/*
	{
		wsv_MCMD_STATUS_REPLY
		m_szName[]
	} X m_btFriendCNT
	*/
} ;


struct cli_MESSENGER_CHAT : public t_PACKETHEADER 
{
	DWORD	m_dwUserTAG;
	char	m_ToName[31];	// (20061102: Nam Byung-Chul), 1:1 chat recipient name
	char	m_szMSG[0];
} ;

struct wsv_MESSENGER_CHAT : public t_PACKETHEADER 
{
	DWORD	m_dwUserTAG;
	char	m_FromName[31];	// (20061102 :Nam Byung-Chul), 1:1 chat recipient name
	char	m_szMSG[0];
} ;

//--------------------------- Chatting room Related packet
#define	CHAT_REQ_ROOM_LIST		0x001
#define	CHAT_REQ_MAKE			0x002
#define	CHAT_REQ_JOIN			0x003
#define	CHAT_REQ_LEFT			0x004
#define	CHAT_REQ_KICK_USER		0x005

#define	MAX_CHAT_ROOM_USERS		64

struct tag_CHAT_HEADER : public t_PACKETHEADER 
{
	BYTE	m_btCMD;
} ;

struct cli_CHAT_ROOM_MAKE : public tag_CHAT_HEADER 
{
	BYTE	m_btRoomTYPE;
	BYTE	m_btMaxUSER;		// Maximum occupancy
	// szTitle[]
	// szPassword[]
} ;
struct cli_CHAT_ROOM_JOIN : public tag_CHAT_HEADER 
{
	BYTE	m_btRoomTYPE;
	WORD	m_wRoomID;
	// szPassword[]
} ;
struct cli_CHAT_ROOM_LIST : public tag_CHAT_HEADER 
{
	BYTE	m_btRoomTYPE;
	WORD	m_wFromRoomID;
} ;
struct cli_CHAT_ROOM_KICK : public tag_CHAT_HEADER 
{
	t_HASHKEY	m_HashUSER;
} ;

struct tag_CHAT_ROOM 
{
	BYTE	m_btRoomTYPE;
	short	m_nRoomIDX;
	char	m_cUserCNT;
	// szTitle[]
} ;
struct wsv_CHAT_ROOM_USER : public tag_CHAT_HEADER 
{
	WORD	m_wUserID;
#ifdef __PRIVATECHAT2
	DWORD	m_dwDBID;	// (20061101: Nam Byung Chul): add individual chat rooms chat
#endif	
	// szName[]
} ;
/*
struct wsv_CHAT_ROOM_JOIN : public tag_CHAT_HEADER 
{
	WORD	m_wUserID;	// Yourself
	szRoomName[]		// Name of room
	{
		WORD	m_wUserID;
		szUserName[];
	} x ???
} ;
*/
struct wsv_CHAT_ROOM_LIST : public tag_CHAT_HEADER 
{
	char	m_cRoomCNT;
	// tag_CHAT_ROOM[]
} ;

#define	CHAT_REPLY_ROOM_MAKED		0x001	// Creating success m_wUserID (myself)
#define	CHAT_REPLY_MAKE_FAILED		0x002

#define	CHAT_REPLY_ROOM_JOINED		0x010	// Participated in the successful wsv_CHAT_ROOM_JOIN []
#define	CHAT_REPLY_FULL_USERS		0x011	// Personnel only land
#define	CHAT_REPLY_INVALIED_PASS	0x012	// Must be off duty
#define CHAT_REPLY_ROOM_NOT_FOUND	0x013	// The room was not
#define CHAT_REPLY_USER_ADD			0x014	// In the add users m_wUserID, szName, []
#define	CHAT_REPLY_KICKED			0x015	// Were deported m_wUserID
#define CHAT_REPLY_USER_LEFT		0x016	// Went out of the room.		m_wUserID
#define	CHAT_REPLY_USER_LEFT_NAME	0x017	// Went out of the room.		To unsubscribe the user name ...
#define CHAT_REPLY_MAKE_TITLE_DUP	0x018	// (20061027: Nam Byung Chul): could not create the room to duplicate prevention

#define	CHAT_REPLY_ROOM_LIST		0x020
#define	CHAT_REPLY_ROOM_LIST_END	0x021

struct cli_CHATROOM_MSG : public t_PACKETHEADER 
{
	char m_szMSG[0];
} ;
struct wsv_CHATROOM_MSG : public t_PACKETHEADER 
{
	WORD m_wObjectID;
	char m_szMSG[0];
} ;


//--------------------------- Note related packet
#define	MEMO_REQ_CONTENTS			0x001	// The complete contents of the note needs to receive
#define MEMO_REQ_SEND				0x002	// Note transport needs
#define MEMO_REQ_RECEIVED_CNT		0x003	// Number of request archived note

struct cli_MEMO : public t_PACKETHEADER 
{
	BYTE m_btTYPE;
	/*
	struct tagReqMemoSend 
	{
		// szTargetCHAR[]
		// szMemoContent[]			// 255 Characters or less
	} ;
	*/
} ;

#define	MEMO_REPLY_RECEIVED_CNT			0x001	// Number of archived note
#define	MEMO_REPLY_CONTENTS				0x002	// The contents of the note.
#define	MEMO_REPLY_SEND_OK				0x003	// Transfer success
#define	MEMO_REPLY_SEND_INVALID_TARGET	0x004	// Send your inquiry to the target error, failure
#define	MEMO_REPLY_SEND_NOT_EXIST		0x005	// The subject of John j.
#define	MEMO_REPLY_SEND_REFUSED			0x006	// Send your inquiry to the State fails, bouncing a note
#define MEMO_REPLY_SEND_FULL_MEMO		0x007	// Failed to send the opponent received so this
#define MEMO_REPLY_SEND_INVALID_CONTENT	0x008	// Content error
struct wsv_MEMO : public t_PACKETHEADER {
	BYTE	m_btTYPE;
	short	m_nRecvCNT[0];						//  m_btTYPE == MEMO_REPLY_RECEIVED_CNT Number of messages when the
	/*
	struct tagReplyMemoCONTENTS 
	{
		// DWORD dwReceivedDATE			// classTIME::GetCurrentAbsSecond()Obtained as a function of time.
		// szFrom[]						// Sent
		// szMemo[]						// Content
	} ;
	*/
} ;


#define	REQ_MALL_ITEM_LIST					0x01	// List request
#define	REQ_MALL_ITEM_FIND_CHAR				0x02	// Jun'ya name check
#define	REQ_MALL_ITEM_BRING					0x03	// To come out
#define	REQ_MALL_ITEM_GIVE					0x04	// To present a
struct cli_MALL_ITEM_REQ : public t_PACKETHEADER 
{
	BYTE	m_btReqTYPE;
	short	m_nDupCnt;								// REQ_MALL_ITEM_BRING, REQ_MALL_ITEM_GIVE Number at. ..
	BYTE	m_btInvIDX[0];							// REQ_MALL_ITEM_GIVE A number from 0 to 39 when an inventory between
	// char m_szCharName[]							// REQ_MALL_ITEM_BRING, REQ_MALL_ITEM_GIVE Request:
	// char m_szDesc[]								// REQ_MALL_ITEM_GIVE Request message (when present):: up to 80 characters
} ;

#define	REPLY_MALL_ITEM_CHECK_CHAR_FOUND	0x01	// Found the target
#define	REPLY_MALL_ITEM_CHECK_CHAR_NONE		0x02	// There is no target.
#define	REPLY_MALL_ITEM_CHECK_CHAR_INVALID	0x03	// His own account, etc. .. Invalid target
#define	REPLY_MALL_ITEM_BRING_SUCCESS		0x04
#define	REPLY_MALL_ITEM_BRING_FAILED		0x05
#define	REPLY_MALL_ITME_GIVE_SUCCESS		0x06
#define	REPLY_MALL_ITEM_GIVE_FAILED			0x07
#define	REPLY_MALL_ITEM_LIST_START			0x08
#define	REPLY_MALL_ITEM_LIST_DATA			0x09
#define	REPLY_MALL_ITEM_LIST_END			0x0a
#define	REPLY_MALL_ITEM_NOT_FOUND			0x0b

struct gsv_MALL_ITEM_REPLY : public t_PACKETHEADER 
{
	BYTE				m_btReplyTYPE;
	BYTE				m_btCntOrIdx;				// 2 bathrooms/Mall inventory index

	/* MALL_ITEM_REQ_LIST Reply: as long as the number of m_nCntOrIdx ..:
		m_btReplyTYPE == REPLY_MALL_ITEM_LIST_START
			m_btReplyTYPE == REPLY_MALL_ITEM_LIST_DATA
			{
				m_btCntOrIdx :: 2 bathrooms
				{
					tagBaseITEM
					szFrom[]		// Presented by Rick Cain
					if ( '\0' != szFrom[0] ) {
						// szDesc[]	// When the gift message
						// szTo[]	// The name of the target presented by Rick Cain
					}
				}
			}
		m_btReplyTYPE == REPLY_MALL_ITEM_LIST_END
	*/
	// REPLY_MALL_ITEM_BRING_SUCCESS Reply:: m_nCntOrIdx m_BringITEM the Mall inventory index [0] added items to the
	union {
		tag_SET_INVITEM	m_BringITEM[0];				// MALL_ITEM_REQ_BRING In response to (change the inventory information): m_btCntOrIdx go to the Mall of the inventory items
	} ;
} ;


#define	BILLING_MSG_FREE_USER				0x00	//

//#define	BILLING_MSG_FEE_TYPE1				0x01	//  /Rates: flat fare system. (June 01, 2004)
//#define	BILLING_MSG_FEE_TYPE2				0x02	//         Your quantitative plan. (18 hours 20 minutes 20 seconds)

#define	BILLING_MSG_PAY_FU					0x01	// FU	Indicates that the user ID of the logged in user for free.
#define	BILLING_MSG_PAY_FA					0x02	// FA	Indicates that the user is logged in, the user your cum.
#define	BILLING_MSG_PAY_FAP					0x03	// FAP	Logged in user is personal cum users (over-tea).
#define	BILLING_MSG_PAY_FQ					0x04	// FQ	Indicates that the user is signed in, individual quantitative user.
#define	BILLING_MSG_PAY_GU					0x05	// GU	Indicates that the user is signed in, games room with users.
#define	BILLING_MSG_PAY_GQ					0x06	// GQ	Tried to sign, but in the games room games room billing period (time), this indicates that your stint as a login has expired.
#define	BILLING_MSG_PAY_IQ					0x07	// IQ	The games room has already been contracted but try to log IP numbers I've been using all of the individual metering has been logged in with.


#define	BILLING_MSG_KICKOUT_CHEAT_DETECT	0x21	// Hodges ended in nuclear, etc. is found cheating
#define	BILLING_MSG_KICKOUT_ALREADY_LOGIN	0x22	// Already have an account with access to La exit
#define	BILLING_MSG_KICKOUT_TIME_EXPIRED	0x23	// Billing time expiration
#define	BILLING_MSG_KICKOUT_DUP_LOGIN		0x24	// Connect the other end of this over dial-in
#define BILLING_MSG_KICKOUT_NO_RIGHT		0x25	// No permission to access ...
#define	BILLING_MSG_KICKOUT_OUT_OF_IP		0x26	// Pc can access the room exceeded the bet by ip
#define	BILLING_MSG_KICKOUT_EXIST_ACCOUNT	0x27	// Cut the connected account.

#define	BILLING_MSG_JPN_NEED_CHARGE			0x41	// Japan billing charge needed!!! Gsv_BILLING_MESSAGE2 m_dwPayFlag packet structure and set the required billing in Flex

//#define	BILLING_MSG_TIME_ALERT				0x31	// Time remaining notice
//#define	BILLING_MSG_TIME_EXPIRED			0x32	// Is the payment time games < end > exit.

#define	BILLING_MSG_TYPE_SET_DATE			0x81	// Set an expiration date, szMsg = 200,309,101,025
#define	BILLING_MSG_TYPE_SET_TIME			0x82	// Set the expiration time, the remaining minutes szMsg =
#define	BILLING_MSG_TYPE_TIME_ALERT			0x83	// Time remaining warning szMsg = remaining minutes

#define	BILLING_MSG_KICKOUT_TAIWAN			0xf0	// Taiwan cut when he was informed of the billing server in the La. ..
#define	BILLING_MSG_TYPE_TAIWAN				0xf1	// Taiwan received a message from the server to the billing ... All you need to do is show as


struct gsv_BILLING_MESSAGE : public t_PACKETHEADER 
{
	BYTE	m_btTYPE;
	// char m_szMsg[];
} ;
struct gsv_BILLING_MESSAGE2: public t_PACKETHEADER 
{
	BYTE	m_btTYPE;
	char	m_cFunctionTYPE;						// ISO classification (A-R)
	DWORD	m_dwPayFlag;
} ;

// South Korea billing payment bit ~
#define	PAY_FLAG_KR_FU				(0x01 << BILLING_MSG_PAY_FU)	// FU	Indicates that the user ID of the logged in user for free.
#define	PAY_FLAG_KR_FA				(0x01 << BILLING_MSG_PAY_FA)	// FA	Indicates that the user is logged in, the user your cum.
#define	PAY_FLAG_KR_FAP				(0x01 << BILLING_MSG_PAY_FAP)	// FAP	Logged in user is personal cum users (over-tea).
#define	PAY_FLAG_KR_FQ				(0x01 << BILLING_MSG_PAY_FQ)	// FQ	Indicates that the user is signed in, individual quantitative user.
#define	PAY_FLAG_KR_GU				(0x01 << BILLING_MSG_PAY_GU)	// GU	Indicates that the user is signed in, games room with users.
#define	PAY_FLAG_KR_GQ				(0x01 << BILLING_MSG_PAY_GQ)	// GQ	Tried to sign, but in the games room games room billing period (time), this indicates that your stint as a login has expired.
#define	PAY_FLAG_KR_IQ				(0x01 << BILLING_MSG_PAY_IQ)	// IQ	The games room has already been contracted but try to log IP numbers I've been using all of the individual metering has been logged in with.

// #define	PAY_FLAG_KR_PLATINUM		( FLAG_PAY_FAP|FLAG_PAY_GU )

// Japan billing payment bit ~
//#define	FLAG_JPAY_CHAT			(0x01 << 0)						// 0	Free Play 	Written by Carrick, chat
#define	PAY_FLAG_JP_BATTLE			(0x01 << 0)	 					// 1	Entry	Battle, Bay expression fields
#define	PAY_FLAG_JP_COMMUNITY		(0x01 << 1)	 					// 2	Community	Friends list, chat room
#define	PAY_FLAG_JP_TRADE			(0x01 << 2)	 					// 3	Trade	Deals, shop
#define	PAY_FLAG_JP_STOCK_SPACE		(0x01 << 3)	 					// 4	Stock Space	Bank
#define	PAY_FLAG_JP_EXTRA_STOCK		(0x01 << 4)	 					// 5	Extra Stock	Extra Bank space
#define	PAY_FLAG_JP_STARSHIP_PASS	(0x01 << 5)	 					// 6	Starship Pass	Interplanetary move
#define	PAY_FLAG_JP_DUNGEON_ADV		(0x01 << 6)	 					// 7	Dungeon Adventure	Dungeon field
#define	PAY_FLAG_JP_EXTRA_CHAR		(0x01 << 7)	 					// 8	Extra Character	It is possible to write Carrick added

#define	PAY_FLAG_JP_ALL				(0xff << 0)						//      All billing payment ~


// Depending on your type of play and check whether charging is a bit ...
// In the case of FLAG_PAY_FAP, FLAG_PAY_GU of Korea billing every bit on below 
// Aside from the penalties South Korea PLAY_FLAG_EXTRA_STOCK, PLAY_FLAG_EXTRA_CHAR bit off
#define	PLAY_FLAG_BATTLE			(0x01 << 0)	 					// 1	Entry	배Framework, the Bay, the expression field
#define	PLAY_FLAG_COMMUNITY			(0x01 << 1)	 					// 2	Community	Friends list, chat room
#define	PLAY_FLAG_TRADE				(0x01 << 2)	 					// 3	Trade	Deals, shop
#define	PLAY_FLAG_STOCK_SPACE		(0x01 << 3)	 					// 4	Stock Space	Bank
#define	PLAY_FLAG_EXTRA_STOCK		(0x01 << 4)	 					// 5	Extra Stock	Extra Bank space
#define	PLAY_FLAG_STARSHIP_PASS		(0x01 << 5)	 					// 6	Starship Pass	Interplanetary move
#define	PLAY_FLAG_DUNGEON_ADV		(0x01 << 6)	 					// 7	Dungeon Adventure	Dungeon field
#define	PLAY_FLAG_EXTRA_CHAR		(0x01 << 7)	 					// 8	Extra Character	It is possible to write Carrick added

// 한국 기본 과금
#define	PLAY_FLAG_KOREA_DEFAULT		( PLAY_FLAG_BATTLE | PLAY_FLAG_COMMUNITY | PLAY_FLAG_TRADE | PLAY_FLAG_STOCK_SPACE | PLAY_FLAG_STARSHIP_PASS | PLAY_FLAG_DUNGEON_ADV )
#define	PLAY_FLAG_TAIWAN_DEFAULT	PLAY_FLAG_KOREA_DEFAULT


#define	EXT_BILLING_MSG_PAY_KOREA			0x1001	// South Korea-billing information
#define	EXT_BILLING_MSG_PAY_JAPAN			0x1002	// Japan billing flag
#define	EXT_BILLING_MSG_PAY_TAIWAN			0x1003	// Taiwan billing type number

#define	EXT_BILLING_MSG_TYPE_SET_DATE		0x1010	// Set an expiration date m_dwPayType = m_dwPlayingFlag = 0 200309101025 = type \ t200309101025\t type \ szMsg t. ..
#define	EXT_BILLING_MSG_TYPE_SET_TIME		0x1011	// Set the expiration time of m_dwPayType = m_dwPlayingFlag = 0, szMsg = type \ t t t type \ remaining minutes remaining minutes \ ...
#define EXT_BILLING_MSG_TYPE_TIME_EXPIRED	0x1012	// PayType = 0 and m_dwPlayingFlag [0] in the billing end on flags
#define EXT_BILLING_MSG_TYPE_TIME_ALERT		0x1013	// PAyType = 0 and m_dwPlayingFlag [0] in time, charges that are warning flags

struct gsv_BILLING_MESSAGE_EXT : public t_PACKETHEADER 
{
	WORD	m_wMsgTYPE;
	DWORD	m_dwPayType;
	DWORD	m_dwPlayingFlag[ 4 ];	// Free to be~ :: 32 * 4 = 128
	// char m_szMsg[];
} ;



//-------------------------------------------------------------------------------------------------
struct gsv_GODDNESS_MODE : public t_PACKETHEADER 
{
	BYTE	m_btOnOff;								// Apply/remove
	WORD	m_wObjectIDX;							// The destination server characters index
} ;

//-------------------------------------------------------------------------------------------------
struct gsv_PATSTATE_CHANGE : public t_PACKETHEADER  // GSV_PATSTATE_CHANGE
{ 
	BYTE	m_btOnOff;								// Cart recall possible/impossible (1/0) 
	DWORD	m_dwCoolTIME;							// It is impossible to summon the cart, here's a cool time value
	short	m_nMaxPatHP;							// Cart fitness
	WORD	m_wObjectIDX;							// Use the cart's stamina 
} ;


//-------------------------------------------------------------------------------------------------
struct gsv_CHARSTATE_CHANGE : public t_PACKETHEADER 
{
	WORD	m_wObjectIDX;
	DWORD	m_dwFLAG;
};


struct cli_CART_RIDE : public t_PACKETHEADER 
{
	BYTE	m_btType;
	WORD	m_wOwnerObjIDX;				// Burn avatar
	WORD	m_wGuestObjIDX;				// Tal-avatar
} ;
struct gsv_CART_RIDE : public cli_CART_RIDE {

} ;
#define	CART_RIDE_REQ				0x01	// Tal?	M_wGuestObjIDX are sent only to
#define	CART_RIDE_ACCEPT			0x02	// Ride after all around sent Rick
#define	CART_RIDE_REFUSE			0x03	// Antalya-Rae m_wOwnerObjIDX are sent only to
#define	CART_RIDE_OWNER_NOT_FOUND	0x04	// Gives IKE was Rick ride gone
#define	CART_RIDE_GUEST_NOT_FOUND	0x05	// Burn dawn jun'ya disappeared


enum EGSV_WARNING_MSGCODE		// m_dwWarningCode Apply to the
{
	EGSV_PARTY_EXP_LIMIT = 0,						// A warning message is limited to acquire an experience party (ex. "there is a difference in the level of the members of the party.")
	EGSV_DONT_DUPLICATE_EXPERIENCE_MILEAGE_ITEM_USE,// Use the duplicate item warning mileage (ex: "can't use duplicate items.")
	EGSV_EXPERIENCE_MILEAGE_ITEM_EXPIRED			// Experience mileage expiring items (ex: "experience the item has expired.")
};

struct gsv_WARNING_MSGCODE : public t_PACKETHEADER 
{
	DWORD	m_dwWarningCode;
};


//-------------------------------------------------------------------------------------------------
//	Azadeh 2006.09.18 added.
struct gsv_cli_USER_MSG_1LV : public t_PACKETHEADER 
{
	WORD	m_Sub_Type;	 // 1: The user has died.
	WORD	m_Data;		// Booking for variables [not yet disabled.]
};


#ifdef __EXPERIENCE_ITEM
struct gsv_MILEAGE_ITEM_TIME : public t_PACKETHEADER
{
	DWORD	dwIndex;	// Mileage item index (categories) (ex: 0 to 2: free, extended, form the warehouse/3: cart gauge/4: experience 2 times)
	WORD	wYear;
	BYTE	btMonth;
	BYTE	btDay;
	BYTE	btHour;
	BYTE	btMin;
};

struct cli_MILEAGE_ITEM_TIME : public t_PACKETHEADER
{
	DWORD	dwIndex;		// Mileage item index (categories) (ex: 0 to 2: free, extended, form the warehouse/3: cart gauge/4: experience 2 times)
	DWORD	dwCharIndex;	// Character index
};
#endif

// 2007.02.13/Kim Dae-Seong/add-in client and server time synchronization
struct cli_SERVER_TIME : public t_PACKETHEADER 
{
	__int64	m_ClientTime;
};
struct gsv_SERVER_TIME : public t_PACKETHEADER 
{
	__int64	m_ClientTime;
	__int64	m_ServerTime;
	int wYear;
	int wMonth;
	int wDay;
	int wHour;
	int wMin;
	int wSec;
};

struct cli_LOGIN_REQ2 : public t_PACKETHEADER 
{
	DWORD	m_dwSeq;
	char szSessionKey[33];
} ;

struct srv_LOGIN_REPLY2 : public t_PACKETHEADER 
{
	char sztxtAccount[36];
	char szMD5PW[33];
} ;

//-------------------------------------------------------------------------------------------------

#if defined(__SERVER) && !defined(__SKIP_SRV_PROTOTYPE)
#include "SRV_Prototype.h"
#endif

//-------------------------------------------------------------------------------------------------
#define	__T_PACKET
struct t_PACKET 
{
	union 
	{
		t_PACKETHEADER				m_HEADER;
		BYTE						m_pDATA[ MAX_PACKET_SIZE ];
		t_NETWORK_STATUS			m_NetSTATUS;

		cli_CHECK_AUTH				m_cli_CHECK_AUTH;
		srv_CHECK_AUTH				m_srv_CHECK_AUTH;

		lsv_ERROR					m_lsv_ERROR;
		gsv_ERROR					m_gsv_ERROR;

		gsv_GM_COMMAND				m_gsv_GM_COMMAND;

		gsv_SET_GLOBAL_VAR			m_gsv_SET_GLOBAL_VAR;
		gsv_SET_GLOBAL_FLAG			m_gsv_SET_GLOBAL_FLAG;

		cli_LOGOUT_REQ				m_cli_LOGOUT_REQ;
		gsv_LOGOUT_REPLY			m_gsv_LOGOUT_REPLY;

		// CLI <-> LSV
		cli_ACCEPT_REQ				m_cli_ACCEPT_REQ;
        cli_LOGIN_REQ				m_cli_LOGIN_REQ;
        srv_LOGIN_REPLY				m_srv_LOGIN_REPLY;

		cli_CHANNEL_LIST_REQ		m_cli_CHANNEL_LIST_REQ;
		lsv_CHANNEL_LIST_REPLY		m_lsv_CHANNEL_LIST_REPLY;

		cli_SELECT_SERVER			m_cli_SELECT_SERVER;
		lsv_SELECT_SERVER			m_lsv_SELECT_SERVER;


		// CLI <-> WSV/GSV
		cli_JOIN_SERVER_REQ			m_cli_JOIN_SERVER_REQ;
		srv_JOIN_SERVER_REPLY		m_srv_JOIN_SERVER_REPLY;
		/*
		cli_JOIN_WORLD				m_cli_JOIN_WORLD;
		wsv_JOIN_WORLD				m_wsv_JOIN_WORLD;
		*/

		// CLI <-> WSV
		wsv_MOVE_SERVER				m_wsv_MOVE_SERVER;

		cli_CHAR_LIST				m_cli_CHAR_LIST;
		wsv_CHAR_LIST				m_wsv_CHAR_LIST;

		cli_CREATE_CHAR				m_cli_CREATE_CHAR;
		wsv_CREATE_CHAR				m_wsv_CREATE_CHAR;

		cli_DELETE_CHAR				m_cli_DELETE_CHAR;
		wsv_DELETE_CHAR				m_wsv_DELETE_CHAR;

		gsv_INIT_DATA				m_gsv_INIT_DATA;

		cli_SELECT_CHAR				m_cli_SELECT_CHAR;
		gsv_SELECT_CHAR				m_gsv_SELECT_CHAR;


		// CLI <-> GSV
		gsv_SERVER_DATA				m_gsv_SERVER_DATA;
		gsv_RELAY_REQ				m_gsv_RELAY_REQ;
		cli_RELAY_REPLY				m_cli_RELAY_REPLY;

		cli_JOIN_ZONE				m_cli_JOIN_ZONE;
		gsv_JOIN_ZONE				m_gsv_JOIN_ZONE;

		cli_REVIVE_REQ				m_cli_REVIVE_REQ;
		gsv_REVIVE_REPLY			m_gsv_REVIVE_REPLY;
		cli_SET_REVIVE_POS			m_cli_SET_REVIVE_POS;

		cli_SET_VAR_REQ				m_cli_SET_VAR_REQ;
		gsv_SET_VAR_REPLY			m_gsv_SET_VAR_REPLY;

		gsv_INVENTORY_DATA			m_gsv_INVENTORY_DATA;
		gsv_QUEST_DATA				m_gsv_QUEST_DATA;


		gsv_QUEST_ONLY				m_gsv_QUEST_ONLY;
		gsv_WISH_LIST				m_gsv_WISH_LIST;

		gsv_CHEAT_CODE				m_gsv_CHEAT_CODE;

		cli_SET_MOTION				m_cli_SET_MOTION;
		gsv_SET_MOTION				m_gsv_SET_MOTION;

		cli_TOGGLE					m_cli_TOGGLE;
		gsv_TOGGLE					m_gsv_TOGGLE;

		cli_CHAT					m_cli_CHAT;
		gsv_CHAT					m_gsv_CHAT;

		cli_WHISPER					m_cli_WHISPER;
		gsv_WHISPER					m_gsv_WHISPER;

		cli_SHOUT					m_cli_SHOUT;
		gsv_SHOUT					m_gsv_SHOUT;

		cli_PARTY_CHAT				m_cli_PARTY_CHAT;
		gsv_PARTY_CHAT				m_gsv_PARTY_CHAT;

		cli_CLAN_CHAT				m_cli_CLAN_CHAT;
		wsv_CLAN_CHAT				m_wsv_CLAN_CHAT;

		cli_ALLIED_CHAT				m_cli_ALLIED_CHAT;
		gsv_ALLIED_CHAT				m_gsv_ALLIED_CHAT;

		cli_ALLIED_SHOUT			m_cli_ALLIED_SHOUT;
		gsv_ALLIED_SHOUT			m_gsv_ALLIED_SHOUT;

		gsv_SET_EVENT_STATUS		m_gsv_SET_EVENT_STATUS;

		tag_ADD_CHAR				m_tag_ADD_CHAR;
		gsv_MOB_CHAR				m_gsv_MOB_CHAR;
		gsv_NPC_CHAR				m_gsv_NPC_CHAR;
		gsv_AVT_CHAR				m_gsv_AVT_CHAR;
		gsv_SUB_OBJECT				m_gsv_SUB_OBJECT;

		cli_SET_WEIGHT_RATE			m_cli_SET_WEIGHT_RATE;
		gsv_SET_WEIGHT_RATE			m_gsv_SET_WEIGHT_RATE;

		gsv_ADJUST_POS				m_gsv_ADJUST_POS;

		cli_CANTMOVE				m_cli_CANTMOVE;
		cli_SETPOS					m_cli_SETPOS;

		cli_STOP					m_cli_STOP;
		gsv_STOP					m_gsv_STOP;

		cli_ATTACK					m_cli_ATTACK;
		gsv_ATTACK					m_gsv_ATTACK;

		cli_DAMAGE					m_cli_DAMAGE;
		gsv_DAMAGE					m_gsv_DAMAGE;

		gsv_SETEXP					m_gsv_SETEXP;
		gsv_LEVELUP					m_gsv_LEVELUP;

		cli_HP_REQ					m_cli_HP_REQ;
		gsv_HP_REPLY				m_gsv_HP_REPLY;

		gsv_SET_HPnMP				m_gsv_SET_HPnMP;

		cli_MOUSECMD				m_cli_MOUSECMD;
		gsv_MOUSECMD				m_gsv_MOUSECMD;

		gsv_MOVE					m_gsv_MOVE;

		gsv_CHANGE_NPC				m_gsv_CHANGE_NPC;

		cli_STORE_TRADE_REQ			m_cli_STORE_TRADE_REQ;
		gsv_STORE_TRADE_REPLY		m_gsv_STORE_TRADE_REPLY;

		gsv_SET_MONEYnINV			m_gsv_SET_MONEYnINV;
		gsv_SET_INV_ONLY			m_gsv_SET_INV_ONLY;

		cli_USE_ITEM				m_cli_USE_ITEM;
		gsv_USE_ITEM				m_gsv_USE_ITEM;

		cli_EQUIP_ITEM				m_cli_EQUIP_ITEM;
		gsv_EQUIP_ITEM				m_gsv_EQUIP_ITEM;

		cli_ASSEMBLE_RIDE_ITEM		m_cli_ASSEMBLE_RIDE_ITEM;
		gsv_ASSEMBLE_RIDE_ITEM		m_gsv_ASSEMBLE_RIDE_ITEM;


		cli_USE_ITEM_TO_REPAIR		m_cli_USE_ITEM_TO_REPAIR;
		gsv_SET_INV_ONLY			m_gsv_USED_ITEM_TO_REPAIR;

		cli_REPAIR_FROM_NPC			m_cli_REPAIR_FROM_NPC;
		gsv_SET_MONEYnINV			m_gsv_REPAIRED_FROM_NPC;

		gsv_SET_ITEM_LIFE			m_gsv_SET_ITEM_LIFE;

		//cli_CHANGE_SKIN				m_cli_CHANGE_SKIN;
		gsv_CHANGE_SKIN				m_gsv_CHANGE_SKIN;

		cli_DROP_ITEM				m_cli_DROP_ITEM;

		cli_MOVE_ITEM				m_cli_MOVE_ITEM;
		gsv_MOVE_ITEM				m_gsv_MOVE_ITEM;

		cli_MOVE_ZULY				m_cli_MOVE_ZULY;
		gsv_MOVE_ZULY				m_gsv_MOVE_ZULY;

		cli_CREATE_ITEM_REQ			m_cli_CREATE_ITEM_REQ;
		gsv_CREATE_ITEM_REPLY		m_gsv_CREATE_ITEM_REPLY;

		//cli_CREATE_ITEM_EXP_REQ		m_cli_CREATE_ITEM_EXP_REQ;

		cli_ITEM_RESULT_REPORT		m_cli_ITEM_RESULT_REPORT;
		gsv_ITEM_RESULT_REPORT		m_gsv_ITEM_RESULT_REPORT;

		cli_BANK_LIST_REQ			m_cli_BANK_LIST_REQ;
		gsv_BANK_LIST_REPLY			m_gsv_BANK_LIST_REPLY;

		gsv_ADD_FIELDITEM			m_gsv_ADD_FIELDITEM;
		gsv_SUB_FIELDITEM			m_gsv_SUB_FIELDITEM;

		cli_GET_FIELDITEM_REQ		m_cli_GET_FIELDITEM_REQ;
		gsv_GET_FIELDITEM_REPLY		m_gsv_GET_FIELDITEM_REPLY;

		cli_TELEPORT_REQ			m_cli_TELEPORT_REQ;
		gsv_TELEPORT_REPLY			m_gsv_TELEPORT_REPLY;

		cli_USE_BPOINT_REQ			m_cli_USE_BPOINT_REQ;
		gsv_USE_BPOINT_REPLY		m_gsv_USE_BPOINT_REPLY;

		cli_SET_HOTICON				m_cli_SET_HOTICON;
		gsv_SET_HOTICON				m_gsv_SET_HOTICON;

		cli_SET_BULLET				m_cli_SET_BULLET;
		gsv_SET_BULLET				m_gsv_SET_BULLET;


		cli_TRADE_P2P				m_cli_TRADE_P2P;
		gsv_TRADE_P2P				m_gsv_TRADE_P2P;

		cli_TRADE_P2P_ITEM			m_cli_TRADE_P2P_ITEM;
		gsv_TRADE_P2P_ITEM			m_gsv_TRADE_P2P_ITEM;

		cli_SET_WISHITEM			m_cli_SET_WISHITEM;

		cli_P_STORE_OPEN			m_cli_P_STORE_OPEN;
		gsv_P_STORE_OPENED			m_gsv_P_STORE_OPENED;

		cli_P_STORE_CLOSE			m_cli_P_STORE_CLOSE;
		gsv_P_STORE_CLOSED			m_gsv_P_STORE_CLOSED;

		cli_P_STORE_LIST_REQ		m_cli_P_STORE_LIST_REQ;
		gsv_P_STORE_LIST_REPLY		m_gsv_P_STORE_LIST_REPLY;

		cli_P_STORE_BUY_REQ			m_cli_P_STORE_BUY_REQ;
		cli_P_STORE_SELL_REQ		m_cli_P_STORE_SELL_REQ;

		gsv_P_STORE_RESULT			m_gsv_P_STORE_RESULT;

		gsv_SKILL_LEARN_REPLY		m_gsv_SKILL_LEARN_REPLY;

		cli_SKILL_LEVELUP_REQ		m_cli_SKILL_LEVELUP_REQ;
		gsv_SKILL_LEVELUP_REPLY		m_gsv_SKILL_LEVELUP_REPLY;

		cli_SELF_SKILL				m_cli_SELF_SKILL;
		gsv_SELF_SKILL				m_gsv_SELF_SKILL;

		cli_TARGET_SKILL			m_cli_TARGET_SKILL;
		gsv_TARGET_SKILL			m_gsv_TARGET_SKILL;

		cli_POSITION_SKILL			m_cli_POSITION_SKILL;
		gsv_POSITION_SKILL			m_gsv_POSITION_SKILL;

		gsv_EFFECT_OF_SKILL			m_gsv_EFFECT_OF_SKILL;
		gsv_DAMAGE_OF_SKILL			m_gsv_DAMAGE_OF_SKILL;
		gsv_RESULT_OF_SKILL			m_gsv_RESULT_OF_SKILL;
		gsv_SKILL_START				m_gsv_SKILL_START;
	
		gsv_SKILL_CANCEL			m_gsv_SKILL_CANCEL;

		gsv_CLEAR_STATUS			m_gsv_CLEAR_STATUS;
		gsv_SPEED_CHANGED			m_gsv_SPEED_CHANGED;

		cli_APPRAISAL_REQ			m_cli_APPRAISAL_REQ;
		gsv_APPRAISAL_REPLY			m_gsv_APPRAISAL_REPLY;

		cli_QUEST_REQ				m_cli_QUEST_REQ;
		gsv_QUEST_REPLY				m_gsv_QUEST_REPLY;

		gsv_CHECK_NPC_EVENT			m_gsv_CHECK_NPC_EVENT;

		cli_PARTY_REQ				m_cli_PARTY_REQ;
		gsv_PARTY_REQ				m_gsv_PARTY_REQ;

		cli_PARTY_REPLY				m_cli_PARTY_REPLY;
		gsv_PARTY_REPLY				m_gsv_PARTY_REPLY;

//		wsv_PARTY_USER				m_wsv_PARTY_USER;

		gsv_PARTY_MEMBER			m_gsv_PARTY_MEMBER;
		gsv_PARTY_LEVnEXP			m_gsv_PARTY_LEVnEXP;
		gsv_CHANGE_OBJIDX			m_gsv_CHANGE_OBJIDX;
		gsv_PARTY_ITEM				m_gsv_PARTY_ITEM;
		cli_PARTY_RULE				m_cli_PARTY_RULE;
		gsv_PARTY_RULE				m_gsv_PARTY_RULE;


		gsv_ADD_EVENTOBJ			m_gsv_ADD_EVENTOBJ;
//		gsv_SUB_EVENTOBJ			m_gsv_SUB_EVENTOBJ;

		cli_CLAN_COMMAND			m_cli_CLAN_COMMAND;
		cli_CLAN_CREATE				m_cli_CLAN_CREATE;
		cli_CLAN_MEMBER_JOBnLEV		m_cli_CLAN_MEMBER_JOBnLEV;
		wsv_CLAN_MEMBER_JOBnLEV		m_wsv_CLAN_MEMBER_JOBnLEV;
		wsv_CLAN_COMMAND			m_wsv_CLAN_COMMAND;
		wsv_RESULT_CLAN_SET			m_wsv_RESULT_CLAN_SET;

		tag_CHAT_HEADER				m_tag_CHAT_HEADER;

		cli_CHAT_ROOM_MAKE			m_cli_CHAT_ROOM_MAKE;
		cli_CHAT_ROOM_JOIN			m_cli_CHAT_ROOM_JOIN;
		cli_CHAT_ROOM_LIST			m_cli_CHAT_ROOM_LIST;
		cli_CHAT_ROOM_KICK			m_cli_CHAT_ROOM_KICK;
									
		wsv_CHAT_ROOM_USER			m_wsv_CHAT_ROOM_USER;
		wsv_CHAT_ROOM_LIST			m_wsv_CHAT_ROOM_LIST;

		cli_CHATROOM_MSG			m_cli_CHATROOM_MSG;
		wsv_CHATROOM_MSG			m_wsv_CHATROOM_MSG;

		cli_MEMO					m_cli_MEMO;
		wsv_MEMO					m_wsv_MEMO;

		tag_MCMD_HEADER				m_tag_MCMD_HEADER;

		cli_MCMD_APPEND_REQ			m_cli_MCMD_APPEND_REQ;
		cli_MCMD_APPEND_REPLY		m_cli_MCMD_APPEND_REPLY;
		cli_MCMD_TAG				m_cli_MCMD_TAG;	
		cli_MCMD_STATUS_REQ			m_cli_MCMD_STATUS_REQ;
									
		wsv_MCMD_APPEND_REQ			m_wsv_MCMD_APPEND_REQ;
		wsv_MCMD_APPEND_ACCEPT		m_wsv_MCMD_APPEND_ACCEPT;
		wsv_MCMD_STATUS_REPLY		m_wsv_MCMD_STATUS_REPLY;
		wsv_MCMD_LIST				m_wsv_MCMD_LIST;	

		cli_MESSENGER_CHAT			m_cli_MESSENGER_CHAT;
		wsv_MESSENGER_CHAT			m_wsv_MESSENGER_CHAT;

		cli_CRAFT_ITEM_REQ			m_cli_CRAFT_ITEM_REQ;
		cli_CRAFT_GEMMING_REQ		m_cli_CRAFT_GEMMING_REQ;
		cli_CRAFT_BREAKUP_REQ		m_cli_CRAFT_BREAKUP_REQ;
		cli_CRAFT_UPGRADE_REQ		m_cli_CRAFT_UPGRADE_REQ;
		cli_CRAFT_DRILL_SOCKET_REQ  m_cli_CRAFT_DRILL_SOKCET_REQ;   //Socket creation request ...

									
		gsv_CRAFT_ITEM_REPLY		m_gsv_CRAFT_ITEM_REPLY;

		gsv_SET_MONEY_ONLY			m_gsv_SET_MONEY_ONLY;
		gsv_SET_ABILITY				m_gsv_SET_ABILITY;

		cli_MALL_ITEM_REQ			m_cli_MALL_ITEM_REQ;
		gsv_MALL_ITEM_REPLY			m_gsv_MALL_ITEM_REPLY;

		gsv_BILLING_MESSAGE			m_gsv_BILLING_MESSAGE;
		gsv_BILLING_MESSAGE_EXT		m_gsv_BILLING_MESSAGE_EXT;
		gsv_BILLING_MESSAGE2		m_gsv_BILLING_MESSAGE2;

		gsv_SET_NPC_SHOW			m_gsv_SET_NPC_SHOW;

		cli_CLANMARK_SET			m_cli_CLANMARK_SET;
		cli_CLANMARK_REQ			m_cli_CLANMARK_REQ;
		wsv_CLANMARK_REPLY			m_wsv_CLANMARK_REPLY;

		cli_CLANMARK_REG_TIME		m_cli_CLANMARK_REG_TIME;
		wsv_CLANMARK_REG_TIME		m_wsv_CLANMARK_REG_TIME;

		gsv_GODDNESS_MODE			m_gsv_GODDNESS_MODE;

		cli_SCREEN_SHOT_TIME		m_cli_SCREEN_SHOT_TIME;
		gsv_SCREEN_SHOT_TIME		m_gsv_SCREEN_SHOT_TIME;
		gsv_CHAR_HPMP_INFO			m_gsv_CHAR_HPMP_INFO;			//HP and MP sent from server
		gsv_CHAR_STAT_INFO			m_gsv_CHAR_STAT_INFO;			//Stats sent from server Attack, def, dodge etc.


		cli_SUMMON_CMD				m_cli_SUMMON_CMD;
		// gsv_SUMMON_CMD			m_gsv_SUMMON_CMD;
		gsv_PATSTATE_CHANGE			m_gsv_PATSTATE_CHANGE;

		gsv_CHARSTATE_CHANGE		m_gsv_CHARSTATE_CHANGE;
		cli_CART_RIDE				m_cli_CART_RIDE;
		gsv_CART_RIDE				m_gsv_CART_RIDE;


		//Mileage related items ... The newly added ...  Packet type ...
		gsv_MILEAGE_INV_DATA		m_gsv_MILEAGE_INV_DATA;
		gsv_DESTROY_MILEAGE_ITEM	m_gsv_DESTROY_MILEAGE_ITEM;		//The item will be destroyed with the expiration of a period of
		gsv_NOTIFY_MILEAGE_ITEM		m_gsv_NOTIFY_MILEAGE_ITEM;      //The information moves the items items ...
		gsv_SET_MILEAGE_INV			m_gsv_SET_MILEAGE_INV;			//Altered mileage Ben.

		cli_EQUIP_MILEAGE_ITEM		m_cli_EQUIP_MILEAGE_ITEM;
		gsv_EQUIP_MILEAGE_ITEM		m_gsv_EQUIP_MILEAGE_ITEM;

		cli_USE_MILEAGE_ITEM		m_cli_USE_MILEAGE_ITEM;			//Use the mileage items
		gsv_USE_MILEAGE_ITEM		m_gsv_USE_MILEAGE_ITEM;             

		cli_DUMMY_1					m_cli_DUMMY_1;

		// Begin - Antonio - 2006-07-24  오후 3:40:13
		// Clan relevant additional
		gsv_cli_CLAN_WAR_OK			m_gsv_cli_ClanWar_OK;			// Clan exhibition. Message
		gsv_cli_CLAN_WAR_TIME		m_gsv_cli_ClanWar_Time;			// Clan relevant time remaining
		gsv_cli_CLAN_WAR_ERR		m_gsv_cli_ClanWar_Err;			// Clancy I related errors
		gsv_cli_CLAN_WAR_PROGRESS	m_gsv_cli_ClanWar_Progress;		//	Clancy I. Message type
		// End   - Antonio

		// Hong Keun clan I reinforcement
		gsv_cli_CLAN_WAR_BOSS_HP      m_gsv_cli_ClanWar_BOSS_HP;     // Clancy, former boss mob HP transmission structures


		gsv_WARNING_MSGCODE			m_gsv_WARNING_MSGCODE;

		//  The user adds the important message.  ----------------------------------------
		gsv_cli_USER_MSG_1LV		m_gsv_User_MSG_1LV;


#ifdef __EXPERIENCE_ITEM
		cli_MILEAGE_ITEM_TIME		m_cli_MIELAGE_ITEM_TIME;		//For the rest of time requests a specific mileage items.
		gsv_MILEAGE_ITEM_TIME		m_gsv_MIELAGE_ITEM_TIME;		
#endif
		gsv_SERVER_TIME				m_gsv_Server_Time;

		cli_LOGIN_REQ2				m_cli_LOGIN_REQ2;
		srv_LOGIN_REPLY2			m_srv_LOGIN_REPLY2;

#if defined(__SERVER) && !defined(__SKIP_SRV_PROTOTYPE)
		gsv_LOG_SQL					m_gsv_LOG_SQL;

		zws_SERVER_INFO				m_zws_SERVER_INFO;

		zws_CONFIRM_ACCOUNT_REQ		m_zws_CONFIRM_ACCOUNT_REQ;
		wls_CONFIRM_ACCOUNT_REPLY	m_wls_CONFIRM_ACCOUNT_REPLY;

		zws_ADD_ACCOUNT				m_zws_ADD_ACCOUNT;
		zws_SUB_ACCOUNT				m_zws_SUB_ACCOUNT;
		zas_SUB_ACCOUNT				m_zas_SUB_ACCOUNT;
		zas_SERVER_TYPE				m_zas_SERVER_TYPE;

		zas_MESSAGE					m_zas_MESSAGE;
		zas_MESSAGE_EXT				m_zas_MESSAGE_EXT;

		zas_KICK_ACCOUNT			m_zas_KICK_ACCOUNT;
		zas_CHECK_ACCOUNT			m_zas_CHECK_ACCOUNT;

		wls_KICK_ACCOUNT			m_lws_KICK_ACCOUNT;

		gsv_BLOCK					m_gsv_BLOCK;

		lsv_CHECK_ACCOUNT			m_lsv_CHECK_ACCOUNT;

		gsv_CHANGE_CHAR				m_gsv_CHANGE_CHAR;

		srv_CHEAT					m_gsv_CHEAT_REQ;
		srv_CHEAT					m_wsv_CHEAT_REQ;

		srv_ACTIVE_MODE				m_srv_ACTIVE_MODE;
		srv_USER_LIMIT				m_srv_USER_LIMIT;
		wls_CHANNEL_LIST			m_wls_CHANNEL_LIST;

		wls_ACCOUNT_LIST			m_wls_ACCOUNT_LIST;
		zws_ACCOUNT_LIST			m_zws_ACCOUNT_LIST;

		srv_SET_WORLD_VAR			m_srv_SET_WORLD_VAR;

		mon_SERVER_STATUS_REQ		m_mon_SERVER_STATUS_REQ;
		srv_SERVER_LIST_REPLY		m_srv_SERVER_LIST_REPLY;
		wls_SERVER_STATUS_REPLY		m_wls_SERVER_STATUS_REPLY;
		gsv_SERVER_STATUS_REPLY		m_gsv_SERVER_STATUS_REPLY;

		tag_ZWS_CLAN_PACKET			m_zws_CLAN_PACKET;
		zws_CREATE_CLAN				m_zws_CREATE_CLAN;
		zws_SET_USER_CLAN			m_zws_SET_USER_CLAN;
		zws_DEL_USER_CLAN			m_zws_DEL_USER_CLAN;

		gsv_ADJ_CLAN_VAR			m_gsv_ADJ_CLAN_VAR;
		wsv_SET_CLAN_VAR			m_wsv_SET_CLAN_VAR;

		

/*
		gsv_ADD_ZONE_LIST			m_gsv_ADD_ZONE_LIST;

		gsv_DEL_ZONE				m_gsv_DEL_ZONE;
		wsv_DEL_ZONE				m_wsv_DEL_ZONE;

		gsv_WARP_USER				m_gsv_WARP_USER;
		srv_DO_TRIGGER_NCPOBJ		m_srv_DO_TRIGGER_NPCOBJ;
		srv_DO_TRIGGER_EVENTOBJ		m_srv_DO_TRIGGER_EVENTOBJ;

		srv_SET_NPCOBJ_VAR			m_srv_SET_NPCOBJ_VAR;
		srv_SET_EVENTOBJ_VAR		m_srv_SET_EVENTOBJ_VAR;

		gsv_LEVEL_UP				m_gsv_LEVEL_UP;
		wsv_PARTY_CMD				m_wsv_PARTY_CMD;
	*/
#endif
	} ;
} ;

//-------------------------------------------------------------------------------------------------
#pragma pack (pop)
#pragma warning( default:4201 )
#pragma warning( default:4200 )
#endif	// __NET_PROTOTYPE_H

