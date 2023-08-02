#include "RankingManager.h"

#include <Raki_imguiMgr.h>
#include <algorithm>

std::array<int, 5> RankingManager::rankingArray = { 6,5,4,3,2 };

RankingManager::~RankingManager()
{
}

void RankingManager::Init()
{

    numSprite.CreateAndSetDivisionUVOffsets(10, 10, 1, 64, 64,
        TexManager::LoadTexture("Resources/Score.png"));


    //�����L���O�\��X����W
    const float EASE_LEFT_OFFSET = 750.0f;
    //�����L���O�\��Y����W
    const float EASE_TOP_OFFSET = 160.0f;
    //�����L���O�\��Y�\���Ԋu
    const float EASE_Y_OFFSET = 72.0f;

    //�C�[�W���O���W�ݒ�
    for (int i = 0; i < rankScoreEase.size(); i++) {
        float ypos = EASE_TOP_OFFSET + (EASE_Y_OFFSET * (i + 1));

        rankScoreEase[i].Init(Rv3Ease::RV3_EASE_TYPE::EASE_CUBIC_INOUT,
            RVector3(1500.f, ypos, 0),
            RVector3(EASE_LEFT_OFFSET, ypos, 0),
            60);
    }

    const float FONT_SIZE = 50.0f;
    //�x�W�G�Ȑ��C�[�W���O���g�p�����t�H���g�����\��
    emphasisEase.Init(RVector3(FONT_SIZE, 0, 0),
        RVector3(FONT_SIZE, 0, 0),
        RVector3(FONT_SIZE * 1.2f, 0, 0),
        30,
        Rv3Ease::RV3_EASE_TYPE::EASE_CUBIC_IN);
    const float FONT_MOVE_OFFSET = 5;
    emphasisMove.Init(RVector3(0, 0, 0),
        RVector3(0, 0, 0),
        RVector3(FONT_MOVE_OFFSET, 0, 0),
        30,
        Rv3Ease::RV3_EASE_TYPE::EASE_CUBIC_IN);

}

void RankingManager::Update()
{
    if (isStartDisplayRank) {
        frame++;

        for (int i = 0; i < 5; i++) {
            if (frame % (15 * (i + 1)) == 0) {
                rankScoreEase[i].Play();
            }
            rankScoreEase[i].Update();
        }
    }

    emphasisEase.Update();
    emphasisMove.Update();
    if (emphasisEase.isEnded()) {
        emphasisEase.Reset();
    }
    if (emphasisMove.isEnded()) {
        emphasisMove.Reset();
    }
}


void RankingManager::Draw2D()
{
    if (!isStartDisplayRank) { return; }

    //�����T�C�Y
    const float FONT_SIZE = 50.f;
    //�X�R�A�̕\���ʒu�I�t�Z�b�g
    const float SCORE_POS_X_OFFSET = 120.f;

    for (int i = 0; i < 5;i++) {
        //����
        numSprite.DrawNumSprite(rankScoreEase[i].GetNowpos().x,
            rankScoreEase[i].GetNowpos().y,
            FONT_SIZE,
            FONT_SIZE,
            i + 1);

        //�����L���O�z��Ƀ��[�U�[�X�R�A������ꍇ�A�����\������
        if (rankingArray[i] == userNewScore) {
            emphasisEase.Play();
            emphasisMove.Play();
            //�X�R�A
            numSprite.DrawNumSprite(rankScoreEase[i].GetNowpos().x + SCORE_POS_X_OFFSET - emphasisMove.nowPos().x,
                rankScoreEase[i].GetNowpos().y - emphasisMove.nowPos().y,
                emphasisEase.nowPos().x,
                emphasisEase.nowPos().x,
                rankingArray[i]);
        }
        else {
            //�X�R�A
            numSprite.DrawNumSprite(rankScoreEase[i].GetNowpos().x + SCORE_POS_X_OFFSET,
                rankScoreEase[i].GetNowpos().y,
                FONT_SIZE,
                FONT_SIZE,
                rankingArray[i]);
        }

    }



    numSprite.Draw();
}

void RankingManager::DebugDraw()
{
    //imgui�Ń����L���O�f�[�^�\��

    ImguiMgr::Get()->StartDrawImgui("rankingdata", 100, 100);

    int i = 1;

    for (auto& r : rankingArray) {
        ImGui::Text("%d : %d", i, r);
        i++;
    }

    ImguiMgr::Get()->EndDrawImgui();
}


void RankingManager::PostScore(int score)
{

    if (!isRankingSend) {
        //�X�R�A���M
        try
        {
            auto serverStatusCode = Post(L"http://localhost:3000/hage_games/", score).wait();
            //����
            if (serverStatusCode == 1 || serverStatusCode == 2) {
                isRankingSend = true;
            }

        }
        catch (const std::exception& e)
        {
            std::cout << "RankingManager | Exception Detected : %s" << e.what() << std::endl;
        }
    }

    userNewScore = score;
}

std::array<int, 5> RankingManager::GetRanking()
{
    if (!isRankingGot) {
        bool isNotConnection = false;

        //�X�R�A�擾
        try
        {
            auto task = Get<json::value>(L"http://localhost:3000/hage_games/");
            const json::value j = task.get();
            auto array = j.as_array();

            // JSON�I�u�W�F�N�g����K�v������؂�o����int�^�̔z��ɑ��
            for (int i = 0; i < array.size(); i++)
            {
                rankingArray[i] = array[i].at(U("score")).as_integer();
            }

            isRankingGot = true;
        }
        catch (const std::exception& e)
        {
            //���݊i�[����Ă��郍�[�J���f�[�^�Ń����L���O�ϓ����s��
            isNotConnection = true;

            std::cout << "RankingManager | Exception Detected : %s" << e.what() << std::endl;
        }

        if (isNotConnection) {
            //�X�R�A�X�V�ł��鏇�ʂ�����
            auto rankPos = std::ranges::find_if_not(rankingArray, [&](int x) {return x > userNewScore; });
            //�X�V�ł���ꍇ
            if (rankPos != rankingArray.end()) {
                std::shift_right(rankPos, rankingArray.end(), 1);
                *rankPos = userNewScore;
            }
        }
    }




    return rankingArray;
}

void RankingManager::StartDisplayResults()
{
    isStartDisplayRank = true;
}

template<class T>
pplx::task<T> RankingManager::Get(const std::wstring& url)
{
    return pplx::create_task([=]
        {
            http_client client(url);
    return client.request(methods::GET); })
        .then([](http_response response)
            {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } });
}

pplx::task<int> RankingManager::Post(const std::wstring& url, int score)
{
    return pplx::create_task([=]
        {
            json::value postData;
    postData[L"score"] = score;

    http_client client(url);
    return client.request(methods::POST, L"", postData.serialize(),
        L"application/json"); })
        .then([](http_response response)
            {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } })
        .then(
            [](json::value json)
            { return json[L"serverStatus"].as_integer(); });
}
