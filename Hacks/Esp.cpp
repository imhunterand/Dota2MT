#include "Esp.h"

#include "../Interfaces.h"
#include "../Settings.h"
#include "../Utils/Draw.h"

static bool GetBox( CBaseEntity* entity, int* x, int* y, int* w, int* h )
{
    CCollisionProperty *collision = entity->C_BaseModelEntity__GetCollideable();
    if( !collision )
        return false;

    const Vector * const originPtr = entity->C_BaseEntity__WorldSpaceCenter();
    const Vector * const minPtr = collision->GetMins();
    const Vector * const maxPtr = collision->GetMaxs();

    if( !originPtr || !minPtr || !maxPtr )
        return false;

    Vector min = *minPtr + *originPtr;
    Vector max = *maxPtr + *originPtr;

    Vector points[] = { Vector(min.x, min.y, min.z),
                        Vector(min.x, max.y, min.z),
                        Vector(max.x, max.y, min.z),
                        Vector(max.x, min.y, min.z),
                        Vector(min.x, min.y, max.z),
                        Vector(min.x, max.y, max.z),
                        Vector(max.x, max.y, max.z),
                        Vector(max.x, min.y, max.z) };

    int maxX, maxY;
    engine->GetScreenSize( maxX, maxY );
    int leftMost = maxX, rightMost = 0, topMost = maxX, bottomMost = 0;

    for( const auto &vec : points ){
        int screenX = 0, screenY = 0;

        if( Draw::WorldToScreen( vec, screenX, screenY, *g_WorldToScreen ) ){
            return false;
        }

        if( screenX < leftMost )
            leftMost = screenX;
        if( screenX > rightMost )
            rightMost = screenX;

        if( screenY < topMost )
            topMost = screenY;
        if( screenY > bottomMost )
            bottomMost = screenY;
    }

    *x = leftMost;
    *y = topMost;
    *w = rightMost - leftMost;
    *h = bottomMost - topMost;

    return true;
}


void ESP::PaintTraverse( IVPanel *thisptr, IVGuiPaintSurface *surface, VPANEL panel, bool force_repaint, bool allow_force ) {

    int max = entitySystem->GetHighestEntityIndex();
    std::vector<Vector> drawSpots = {};

    for( int i = 1; i <=max; i++ ){
        bool goodToGo = false;
        CBaseEntity *entity = entitySystem->GetBaseEntity(i);

        if( !entity )
            continue;

        if( mc_esp_filter_all->GetBool() )
            goodToGo = true;

        if( entity->C_BaseEntity__InLocalTeam() ){
            if( strstr(entity->Schema_DynamicBinding()->binaryName, "DOTA_Unit_Hero") != nullptr ){
                if( mc_esp_filter_heroes_friend->GetBool() ) {
                    goodToGo = true;
                }
                if( mc_esp_filter_heroes_friend_seenby_enemy->GetBool() ){
                    DOTATeam_t enemyTeam = (entity->GetTeam() == DOTATeam_t::DOTA_TEAM_GOODGUYS) ? DOTATeam_t::DOTA_TEAM_BADGUYS : DOTA_TEAM_GOODGUYS;
                    if( entity->C_BaseModelEntity__CanBeSeenByTeam( enemyTeam ) )
                        goodToGo = true;
                }
            } else if( strstr(entity->Schema_DynamicBinding()->binaryName, "C_DOTA_BaseNPC_Creep") != nullptr ){
                if( mc_esp_filter_creeps_friend->GetBool() )
                    goodToGo = true;
            }
        } else {
            if( strstr(entity->Schema_DynamicBinding()->binaryName, "DOTA_Unit_Hero") != nullptr ){
                if( mc_esp_filter_heroes_enemy->GetBool() ){
                    goodToGo = true;
                }
                if( mc_esp_filter_illusions_enemy->GetBool() ){
                    auto *npc = (CDotaBaseNPC*)entity;
                    if( npc->IsIllusion() ){
                        goodToGo = true;
                    }
                }
            }
            if( mc_esp_filter_creeps_enemy->GetBool() && strstr(entity->Schema_DynamicBinding()->binaryName, "C_DOTA_BaseNPC_Creep") != nullptr )
                goodToGo = true;
        }

        if( !goodToGo )
            continue;

        if( mc_esp_type_infotext->GetBool() ){
            if( !entity->Schema_DynamicBinding() || !entity->Schema_DynamicBinding()->binaryName )
                continue;

            int x, y, w, h;
            if( !GetBox(entity, &x, &y, &w, &h) ){
                continue;
            }

            int sizeX, sizeY;
            const int bufferLen = 512;
            wchar_t buffer[bufferLen];
            int strLen = 0;

            strLen += swprintf( buffer + strLen, std::max( 0, bufferLen - strLen ), L"(%d)%s", i, entity->Schema_DynamicBinding()->binaryName );


            if( strstr( entity->Schema_DynamicBinding()->binaryName, "C_DOTAPlayer" ) != nullptr ){
                auto *player = (CDotaPlayer*)entity;
                strLen += swprintf( buffer + strLen, std::max( 0, bufferLen - strLen ), L" - %s", "GetPlayerName Broke :)"/*player->C_DOTAPlayer__GetPlayerName()*/ );
                strLen += swprintf( buffer + strLen, std::max( 0, bufferLen - strLen ), L" - (%p)", (void*)player );
            }

            if( strstr( entity->Schema_DynamicBinding()->binaryName, "DOTA_Unit_Hero" ) ){
                auto *npc = (CDotaBaseNPC*)entity;
                strLen += swprintf( buffer + strLen, std::max( 0, bufferLen - strLen ), L" -Mana: (%g/%g)", npc->C_DOTA_BaseNPC__GetMana(), npc->C_DOTA_BaseNPC__GetMaxMana() );
                strLen += swprintf( buffer + strLen, std::max( 0, bufferLen - strLen ), L" -Dmg Min(%d)/Max(%d)", npc->C_DOTA_BaseNPC__GetDamageMin(), npc->C_DOTA_BaseNPC__GetDamageMax() );
                strLen += swprintf( buffer + strLen, std::max( 0, bufferLen - strLen ), L" -MagicImm?(%s)", npc->C_DOTA_BaseNPC__IsMagicImmune() ? "yes" : "no" );
                strLen += swprintf( buffer + strLen, std::max( 0, bufferLen - strLen ), L" -Illu?(%s)", npc->IsIllusion() ? "yes" : "no" );
                strLen += swprintf( buffer + strLen, std::max( 0, bufferLen - strLen ), L" -Pos(%f,%f,%f)", npc->C_BaseEntity__WorldSpaceCenter()->x,
                                    npc->C_BaseEntity__WorldSpaceCenter()->y,
                                    npc->C_BaseEntity__WorldSpaceCenter()->z);
            }

            surface->DrawSetTextFont(ESP::paintFont);
            surface->DrawSetTextColor(255, 255, 255, 255);
            surface->GetTextSize(ESP::paintFont, buffer, sizeX, sizeY);

            int objectsInArea = 1;

            for( const auto &spot : drawSpots ){
                if( entity->C_BaseEntity__WorldSpaceCenter() && entity->C_BaseEntity__WorldSpaceCenter()->DistTo(spot) < 50.0f){
                    objectsInArea++;
                }
            }

            surface->DrawSetTextPos(x + (w / 2) - (sizeX / 2), y + ( sizeY * objectsInArea ) + h);
            surface->DrawPrintText(buffer, strLen);

            if( entity->C_BaseEntity__WorldSpaceCenter() ){
                drawSpots.push_back( *(entity->C_BaseEntity__WorldSpaceCenter()) );
            }
        }

        auto *npc = (CDotaBaseNPC*)entity;

        if( mc_esp_type_bbox->GetBool() )
            npc->C_BaseEntity__DrawEntityDebugOverlays(OverlayFlags_t::BBOX);
        if( mc_esp_type_absbox->GetBool() )
            npc->C_BaseEntity__DrawEntityDebugOverlays(OverlayFlags_t::ABSBOX);
        if( mc_esp_type_rbox->GetBool() )
            npc->C_BaseEntity__DrawEntityDebugOverlays(OverlayFlags_t::RBOX);
        if( mc_esp_type_entbounds->GetBool() )
            npc->C_BaseEntity__DrawEntityDebugOverlays(OverlayFlags_t::ENTITYBOUNDS);
        if( mc_esp_type_entattachments->GetBool() )
            npc->C_BaseEntity__DrawEntityDebugOverlays(OverlayFlags_t::ENTITYATTACHMENTS);
        if( mc_esp_type_entjointinfo->GetBool() )
            npc->C_BaseEntity__DrawEntityDebugOverlays(OverlayFlags_t::ENTITYJOINTINFO);
        if( mc_esp_type_entskele->GetBool() )
            npc->C_BaseEntity__DrawEntityDebugOverlays(OverlayFlags_t::ENTITYSKELETON);
        if( mc_esp_type_enthitboxes->GetBool() )
            npc->C_BaseEntity__DrawEntityDebugOverlays(OverlayFlags_t::ENTITYHITBOXES);
        if( mc_esp_type_entviewoffset->GetBool() )
            npc->C_BaseEntity__DrawEntityDebugOverlays(OverlayFlags_t::ENTITYVIEWOFFSET);
    }

}