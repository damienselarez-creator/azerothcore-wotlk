/* Custom reward for the personal WotLK realm. */
#include "Chat.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "Item.h"
#include "Log.h"
#include "Mail.h"
#include "MailMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "WorldSession.h"
#include <memory>
#include <string_view>

namespace
{
enum TharlReward : uint32
{
    NPC_THARL = 16076,
    NPC_RANSIN = 2943,
    ITEM_GLOUBIE = 22114,
    ITEM_BOURBIE = 20371,
    ITEM_OURSON = 44819,
    ITEM_PANDA = 13583,
    ITEM_POLEY = 22781,
    ITEM_GROGNIE = 46802,
    ITEM_TYRAEL = 39656,
    ITEM_DIABLO = 13584,
    ITEM_ZERGLING = 13582,
    ITEM_NEANT = 25535,
    ITEM_FRIGY = 39286,
    ACTION_REDEEM_PET = GOSSIP_ACTION_INFO_DEF + 1,
    MAIL_LIFETIME = 30 * DAY
};

struct PetReward
{
    std::string_view code;
    uint32 item;
    char const* name;
};

constexpr PetReward Rewards[] =
{
    { "GLOUBIE", ITEM_GLOUBIE, "Gloubie" },
    { "BOURBIE", ITEM_BOURBIE, "Bourbie" },
    { "OURSON", ITEM_OURSON, "Ourson du Blizzard" },
    { "PANDA", ITEM_PANDA, "Panda" },
    { "POLEY", ITEM_POLEY, "Poley" },
    { "GROGNIE", ITEM_GROGNIE, "Grognie" },
    { "TYRAEL", ITEM_TYRAEL, "Mini Tyraël" },
    { "DIABLO", ITEM_DIABLO, "Mini Diablo" },
    { "ZERGLING", ITEM_ZERGLING, "Zergling" },
    { "NEANT", ITEM_NEANT, "Dragonnet du Néant" },
    { "FRIGY", ITEM_FRIGY, "Frigy" }
};

bool IsRewardNpc(Creature const* creature)
{
    return creature->GetEntry() == NPC_THARL || creature->GetEntry() == NPC_RANSIN;
}

bool MatchesRewardCode(std::string_view code, std::string_view expected)
{
    auto const first = code.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos)
        return false;
    code = code.substr(first, code.find_last_not_of(" \t\r\n") - first + 1);
    if (code.size() != expected.size())
        return false;
    for (std::size_t i = 0; i < code.size(); ++i)
    {
        char ch = code[i];
        if (ch >= 'a' && ch <= 'z')
            ch = char(ch - 'a' + 'A');
        if (ch != expected[i])
            return false;
    }
    return true;
}

void Notify(Player* player, char const* text)
{
    ChatHandler(player->GetSession()).SendSysMessage(text);
}
}

// CreatureScript handles the coded selection and returns true to suppress the
// default gossip handler. The shared menu 7034 is deliberately left unchanged.
class npc_tharl_gloubie : public CreatureScript
{
public:
    npc_tharl_gloubie() : CreatureScript("npc_tharl_gloubie") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        // The client routes an empty code through the ordinary selection hook.
        return OnGossipSelectCode(player, creature, sender, action, "");
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!IsRewardNpc(creature))
            return false;
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Je souhaite saisir mon code pour recevoir un familier.",
            GOSSIP_SENDER_MAIN, ACTION_REDEEM_PET,
            "Saisissez votre code. Chaque familier peut être reçu une seule fois par compte.", 0, true);
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature);
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender,
        uint32 action, char const* code) override
    {
        ClearGossipMenuFor(player);
        CloseGossipMenuFor(player);
        if (!IsRewardNpc(creature) || sender != GOSSIP_SENDER_MAIN || action != ACTION_REDEEM_PET)
            return true;
        uint32 rewardItem = 0;
        std::string rewardName;
        if (code)
        {
            for (auto const& reward : Rewards)
            {
                if (MatchesRewardCode(code, reward.code))
                {
                    rewardItem = reward.item;
                    rewardName = reward.name;
                    break;
                }
            }
        }
        if (!rewardItem)
        {
            Notify(player, "ce code est incorrect. Vérifiez-le et reparlez-moi.");
            return true;
        }

        uint32 const accountId = player->GetSession()->GetAccountId();
        auto* check = CharacterDatabase.GetPreparedStatement(CHAR_SEL_THARL_REWARD);
        check->SetData(0, accountId);
        check->SetData(1, rewardItem);
        // The aggregate always returns one row. A missing result is a database
        // failure, not permission to issue another reward.
        PreparedQueryResult result = CharacterDatabase.Query(check);
        if (!result)
        {
            Notify(player, "le registre est indisponible. Réessayez plus tard.");
            return true;
        }
        if (result->Fetch()[0].Get<uint64>() != 0)
        {
            std::string const message = "votre compte a déjà reçu " + rewardName
                + ". Vérifiez le courrier du premier personnage.";
            Notify(player, message.c_str());
            return true;
        }

        std::unique_ptr<Item> egg(Item::CreateItem(rewardItem, 1, player));
        if (!egg)
        {
            Notify(player, "je ne peux pas préparer votre cadeau pour le moment. Réessayez plus tard.");
            return true;
        }

        auto const receiver = player->GetGUID().GetCounter();
        auto const itemGuid = egg->GetGUID().GetCounter();
        uint32 const mailId = sObjectMgr->GenerateMailID();
        uint32 const deliveredAt = uint32(GameTime::GetGameTime().count());
        uint32 const expiresAt = deliveredAt + MAIL_LIFETIME;
        std::string const subject = "Votre compagnon " + rewardName;
        std::string const body = "Voici votre cadeau. Utilisez l'objet joint pour apprendre à invoquer "
            + rewardName + ". Prenez soin de votre compagnon !\n\n"
            + (creature->GetEntry() == NPC_THARL ? "Tharl Saignepierre" : "Ransin Donner");

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        auto* claim = CharacterDatabase.GetPreparedStatement(CHAR_INS_THARL_REWARD);
        claim->SetData(0, accountId);
        claim->SetData(1, receiver);
        claim->SetData(2, mailId);
        claim->SetData(3, deliveredAt);
        claim->SetData(4, rewardItem);
        // First statement generates the receipt ID. The UNIQUE (account, item)
        // key protects each reward independently, including concurrent claims.
        trans->Append(claim);
        egg->SaveToDB(trans);

        auto* mail = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL);
        mail->SetData(0, mailId);
        mail->SetData(1, uint8(MAIL_CREATURE));
        mail->SetData(2, int8(MAIL_STATIONERY_DEFAULT));
        mail->SetData(3, uint16(0));
        mail->SetData(4, creature->GetEntry());
        mail->SetData(5, receiver);
        mail->SetData(6, subject);
        mail->SetData(7, body);
        mail->SetData(8, true);
        mail->SetData(9, expiresAt);
        mail->SetData(10, deliveredAt);
        mail->SetData(11, uint32(0));
        mail->SetData(12, uint32(0));
        mail->SetData(13, uint8(MAIL_CHECK_MASK_HAS_BODY));
        trans->Append(mail);

        auto* attachment = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL_ITEM);
        attachment->SetData(0, mailId);
        attachment->SetData(1, itemGuid);
        attachment->SetData(2, receiver);
        trans->Append(attachment);

        // Confirm the entire transaction before exposing the item in memory.
        // Never automatically retry an uncertain commit: the persisted UNIQUE
        // key is authoritative even if confirmation was lost after COMMIT.
        if (!CharacterDatabase.DirectCommitTransactionConfirmed(trans))
        {
            Notify(player, "l'envoi n'a pas été confirmé. Reconnectez-vous et vérifiez votre courrier.");
            return true;
        }

        auto delivered = std::make_unique<Mail>();
        delivered->messageID = mailId;
        delivered->messageType = MAIL_CREATURE;
        delivered->stationery = MAIL_STATIONERY_DEFAULT;
        delivered->mailTemplateId = 0;
        delivered->sender = creature->GetEntry();
        delivered->receiver = receiver;
        delivered->subject = subject;
        delivered->body = body;
        delivered->expire_time = expiresAt;
        delivered->deliver_time = deliveredAt;
        delivered->money = 0;
        delivered->COD = 0;
        delivered->checked = MAIL_CHECK_MASK_HAS_BODY;
        delivered->state = MAIL_STATE_UNCHANGED;
        delivered->AddItem(itemGuid, rewardItem);
        player->AddMail(delivered.release());
        player->AddMItem(egg.release());
        player->AddNewMailDeliverTime(deliveredAt);
        sMailMgr->OnMailSent(receiver);
        std::string const message = "Code accepté ! Le cadeau pour " + rewardName
            + " vous attend dans votre boîte aux lettres.";
        Notify(player, message.c_str());
        return true;
    }
};

void AddSC_npc_tharl_gloubie()
{
    new npc_tharl_gloubie();
    LOG_INFO("server.loading", "Promotional pets: 11 codes registered for Tharl and Ransin (once per account and pet).");
}
